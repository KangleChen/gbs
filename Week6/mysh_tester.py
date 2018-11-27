# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./mysh_tester.py
# Compiled at: 2018-11-25 18:48:11
from __future__ import print_function
import subprocess as sp, os
DEBUG = False
POINTS = 0.0
TOTAL_POINTS = 3.0
PROG_NAME = './mysh'
PASSED = 'Test PASSED: {} [{}/{} pts]'
FAILED = 'Test FAILED: {} [{}/{} pts]'
TESTS = [
 {'name': 'exit on "exit"', 'export': '', 
    'timeout': 5, 
    'input': 'exit\n', 
    'output': [
             '$ '], 
    'pts': 0.5},
 {'name': 'variable', 'export': 'GBS_TEST=VariableValue', 
    'timeout': 5, 
    'input': 'echo $GBS_TEST\nexit\n', 
    'output': [
             '$ VariableValue\n',
             '$ '], 
    'pts': 0.5},
 {'name': 'quotation', 'export': 'export GBS_TEST=VariableValue', 
    'timeout': 5, 
    'input': 'echo "Hallo \'Welt\'" "$GBS_TEST" "let\'s go"\nexit\n', 
    'output': [
             "$ Hallo 'Welt' VariableValue let's go\n",
             '$ '], 
    'pts': 0.5},
 {'name': 'backslash', 'export': 'GBS_TEST=VariableValue', 
    'timeout': 5, 
    'input': 'echo \\$GBS_TEST \\"\\\'abc\\\'\\" \\\\$GBS_TEST\nexit\n', 
    'output': [
             '$ $GBS_TEST "\'abc\'" \\VariableValue\n',
             '$ '], 
    'pts': 0.5},
 {'name': 'backslash space', 'export': '', 
    'timeout': 5, 
    'input': 'echo very\\ long\\ file\\ name\\ with\\ spaces\nexit\n', 
    'output': [
             '$ very long file name with spaces\n',
             '$ '], 
    'pts': 0.5},
 {'name': 'backslash backslash', 'export': '', 
    'timeout': 5, 
    'input': 'echo \\\\"Ich will lernen"\nexit\n', 
    'output': [
             '$ \\Ich will lernen\n',
             '$ '], 
    'pts': 0.5}]

def build_cmd(test):
    cmd = ''
    if test['export']:
        cmd += 'export ' + test['export'] + '; '
    cmd += 'timeout ' + str(test['timeout']) + ' ' + PROG_NAME
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def exec_test(test):
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    prog.stdin.write(test['input'])
    output = prog.stdout.readlines()
    if prog.wait() == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if DEBUG:
        print('output: ', end='')
        print(output)
    if len(output) != len(test['output']):
        print('Wrong number of parameter:')
        print('Expectet: ' + str(len(test['output'])))
        print('Got: ' + str(len(output)))
        fail_test(test)
        return
    for i in range(len(output)):
        if output[i] != test['output'][i]:
            print('Wrong parameter ' + str(i + 1))
            print('  --Expectet--\n' + test['output'][i])
            print('  --Got--\n' + output[i])
            fail_test(test)
            return

    pass_test(test)


def fail_test(test):
    print(FAILED.format(test['name'], 0, test['pts']))


def pass_test(test):
    global POINTS
    POINTS += test['pts']
    print(PASSED.format(test['name'], test['pts'], test['pts']))


def main():
    for test in TESTS:
        exec_test(test)

    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()