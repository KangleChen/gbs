# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./forkone_tester.py
# Compiled at: 2018-11-04 20:15:01
from __future__ import print_function
import subprocess as sp, datetime, time

DEBUG = False
TIP = True
TOTAL_POINTS = 3
POINTS = 0
PASSED = 'Test PASSED: {}'
FAILED = 'Test FAILED: {}'
DATEFORMAT = '%a %b %d %H:%M:%S %Y'
PROG_NAME = './forkone'
TESTS = [
    {'name': 'case 1', 'cmd': PROG_NAME + ' 10',
     'arg': 10,
     'pts': 1},
    {'name': 'case 2', 'cmd': PROG_NAME + ' 5',
     'arg': 5,
     'pts': 1},
    {'name': 'case 3', 'cmd': PROG_NAME + ' 3',
     'arg': 3,
     'pts': 1}]


def exec_test(test_case):
    prog = sp.Popen(test_case['cmd'], stdin=sp.PIPE, stdout=sp.PIPE, shell=True)
    time.sleep(0.5)
    if prog.poll() is not None:
        if DEBUG or TIP:
            print('terminated immediatly')
        fail_test(test_case)
        return
    pids_cmd = 'ps f | grep ' + PROG_NAME + ' | grep -v grep | grep -v "sh -c"'
    pids_ret = sp.Popen(pids_cmd, stdout=sp.PIPE, shell=True).communicate()[0]
    pids = []
    for string in pids_ret.decode('utf-8').split('\n')[:-1]:
        if DEBUG:
            print(string)
        pids.append(int(string.split()[0]))

    if DEBUG:
        print('pids: ' + str(pids))
    ppid = None
    pid = None
    parent_cmd = 'ps -o ppid= -p ' + str(pids[1])
    parent_ret = sp.Popen(parent_cmd, stdout=sp.PIPE, shell=True).communicate()[0]
    parent = int(parent_ret)
    if pids[0] == parent:
        ppid = pids[0]
        pid = pids[1]
    if DEBUG:
        print('ppid: ' + str(ppid))
        print('pid: ' + str(pid))
    raw_output = prog.stdout.readlines()
    output = []
    for line in raw_output:
        output.append(line.decode('utf-8')[:-1])

    if DEBUG:
        for line in output:
            print(line)

    try:
        start_split = output[0].split()
        start_str = (' ').join(start_split[1:])
        start = datetime.datetime.strptime(start_str, DATEFORMAT)
    except Exception as e:
        if DEBUG or TIP:
            print('First line has wrong format')
        fail_test(test_case)
        return

    try:
        end_split = output[-1].split()
        end_str = (' ').join(end_split[1:])
        end = datetime.datetime.strptime(end_str, DATEFORMAT)
    except Exception as e:
        if DEBUG or TIP:
            print('Last line has wrong format')
        fail_test(test_case)
        return

    timediff = int((end - start).total_seconds())
    if not test_case['arg'] - 1 <= timediff <= test_case['arg'] + 1:
        if DEBUG or TIP:
            print('wrong timediff')
        fail_test(test_case)
        return
    counter = 0
    for line in output:
        line_split = line.split()
        if line_split[0] == 'Start:' or line_split[0] == 'Ende:':
            continue
        if line_split[0] == 'Exit-Code:':
            if int(line_split[1]) != (pid + test_case['arg']) % 100:
                if DEBUG or TIP:
                    print('wrong exit code')
                fail_test(test_case)
                return
            continue
        if int(line_split[0]) != pid:
            if DEBUG or TIP:
                print('wrong pid')
            fail_test(test_case)
            return
        if int(line_split[1]) != ppid:
            if DEBUG or TIP:
                print('wrond ppid')
            fail_test(test_case)
            return
        if int(line_split[2]) != counter + 1:
            if DEBUG or TIP:
                print('wrong count')
            fail_test(test_case)
            return
        counter += 1

    if counter != test_case['arg']:
        if DEBUG or TIP:
            print('wrong final count')
        fail_test(test_case)
        return
    pass_test(test_case)
    return


def fail_test(test_case):
    print(FAILED.format(test_case['name']))


def pass_test(test_case):
    global POINTS
    print(PASSED.format(test_case['name']))
    POINTS += test_case['pts']


def main():
    for test in TESTS:
        exec_test(test)

    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()
