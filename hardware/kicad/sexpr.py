"""Kleiner Leser/Schreiber für KiCad-S-Ausdrücke (Symbole, Schaltpläne, Footprints)."""
import re


class Q(str):
    """Zeichenkette in Anführungszeichen. Der Inhalt bleibt roh (Escapes wie in der Datei)."""


_TOK = re.compile(r'\(|\)|"(?:\\.|[^"\\])*"|[^\s()]+')


def parse(text):
    stack = [[]]
    for t in _TOK.findall(text):
        if t == '(':
            stack.append([])
        elif t == ')':
            x = stack.pop()
            stack[-1].append(x)
        elif t.startswith('"'):
            stack[-1].append(Q(t[1:-1]))
        else:
            stack[-1].append(t)
    return stack[0][0] if len(stack[0]) == 1 else stack[0]


def q(s):
    """Eigene Zeichenkette -> Q mit Escapes."""
    return Q(str(s).replace('\\', '\\\\').replace('"', '\\"'))


def dump(x, ind=0):
    if isinstance(x, list):
        parts = [dump(e, ind + 1) for e in x]
        one = '(' + ' '.join(parts) + ')'
        if len(one) <= 110 and '\n' not in one:
            return one
        pad = '\n' + '  ' * (ind + 1)
        return '(' + parts[0] + ''.join(pad + p for p in parts[1:]) + ')'
    if isinstance(x, Q):
        return '"' + x + '"'
    if isinstance(x, float):
        s = ('%.4f' % x).rstrip('0').rstrip('.')
        return s if s not in ('-0', '') else '0'
    return str(x)


def find(node, key):
    """Erstes Kind-Element (Liste) mit dem Kopf key."""
    for e in node:
        if isinstance(e, list) and e and e[0] == key:
            return e
    return None


def find_all(node, key):
    return [e for e in node if isinstance(e, list) and e and e[0] == key]
