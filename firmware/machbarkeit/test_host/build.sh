#!/bin/sh
# Prüft die Test-Firmware auf dem PC:
#  1. Syntaxprüfung beider Sketche gegen Ersatz-Header (stubs/) mit den Original-Signaturen
#  2. Simulationstest der Signalverarbeitung (lockin.h)
set -e
cd "$(dirname "$0")"
for sk in sender_test empfaenger_test; do
  g++ -std=gnu++17 -fsyntax-only -Wall -Wextra -Wno-unused-parameter -Istubs -I../$sk -include Arduino.h -x c++ ../$sk/$sk.ino
  echo "Syntax OK: $sk"
done
g++ -O2 -std=c++17 -Wall -Wextra sim_test.cpp -o /tmp/kabelsucher_sim_test
/tmp/kabelsucher_sim_test
