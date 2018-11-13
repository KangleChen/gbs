# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./threadit_tester.py
# Compiled at: 2018-11-12 08:11:38
from __future__ import print_function
import subprocess as sp, datetime, time

DEBUG = False
TIP = True
TOTAL_POINTS = 3
POINTS = 0
PASSED = 'Test PASSED: {}'
FAILED = 'Test FAILED: {}'
DATEFORMAT = '%a %b %d %H:%M:%S %Y'
PROG_NAME = './threadit'
TESTS = [
    {'name': 'one thread, fixed length', 'cmd': PROG_NAME + ' -k 10 -n 1',
     'k': 10,
     'n': 1, 'r': False, 'pts': 0.5},
    {'name': 'random', 'cmd': PROG_NAME + ' -r',
     'k': 10,
     'n': 1, 'r': True, 'pts': 0.5},
    {'name': 'only n', 'cmd': PROG_NAME + ' -n 6',
     'k': 10,
     'n': 6, 'r': False, 'pts': 0.5},
    {'name': 'only k', 'cmd': PROG_NAME + ' -k 4',
     'k': 4,
     'n': 1, 'r': False, 'pts': 0.5},
    {'name': 'multiple threads, fixed length', 'cmd': PROG_NAME + ' -k 10 -n 10',
     'k': 10,
     'n': 10, 'r': False, 'pts': 0.5},
    {'name': 'multiple threads, random length', 'cmd': PROG_NAME + ' -k 10 -n 10 -r',
     'k': 10,
     'n': 10, 'r': True, 'pts': 0.5}]


def exec_test(test_case):
    prog = sp.Popen(test_case['cmd'], stdin=sp.PIPE, stdout=sp.PIPE, shell=True)
    time.sleep(0.5)
    if prog.poll() is not None:
        if DEBUG or TIP:
            print('terminated immediatly')
        fail_test(test_case)
        return
    pid_cmd = 'ps f | grep ' + PROG_NAME + ' | grep -v grep | grep -v "sh -c"'
    pid_ret = sp.Popen(pid_cmd, stdout=sp.PIPE, shell=True).communicate()[0]
    pid = int(pid_ret.split()[0])
    if DEBUG:
        print(pid_ret, end='')
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
    if not test_case['r']:
        if not test_case['k'] - 1 <= timediff <= test_case['k'] + 1:
            if DEBUG or TIP:
                print('wrong timediff')
            fail_test(test_case)
            return
    else:
        if not test_case['k'] / 2 <= timediff <= test_case['k'] * 1.5:
            if DEBUG or TIP:
                print('wrong random timediff')
            fail_test(test_case)
            return
    count_dict = {}
    exit_codes = []
    for line in output:
        line_split = line.split()
        if line.startswith('1:') or not line:
            continue
        if line_split[0] == 'Start:' or line_split[0] == 'Ende:':
            continue
        if line_split[0] == 'Exit-Code:':
            exit_codes.append(int(line_split[1]))
            continue
        if int(line_split[1]) != pid:
            if DEBUG or TIP:
                print('wrond ppid (' + str(pid) + ';' + line_split[1] + ')');
            fail_test(test_case)
            return
        if not update_dict(count_dict, int(line_split[0]), int(line_split[2])):
            if DEBUG or TIP:
                print('wrong count')
            fail_test(test_case)
            return

    for pid in count_dict:
        if test_case['r']:
            if not test_case['k'] / 2 <= count_dict[pid] <= test_case['k'] * 1.5:
                if DEBUG or TIP:
                    print('wrong random final count')
                fail_test(test_case)
                return
        else:
            if count_dict[pid] != test_case['k']:
                if DEBUG or TIP:
                    print('wrong final count')
                fail_test(test_case)
                return

    if len(count_dict) != test_case['n']:
        if DEBUG or TIP:
            print('wrong number of children')
        fail_test(test_case)
        return
    pass_test(test_case)
    return


def update_dict(count_dict, pid, count):
    """
    updates the count_dict with the new value for the pid
    :return: True if it was a valid assignment, False if it was invalid
    """
    if pid in count_dict:
        if count_dict[pid] + 1 == count:
            count_dict[pid] = count
            return True
    else:
        if count == 1:
            count_dict[pid] = count
            return True
    return False


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
