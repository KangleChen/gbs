# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./threadsched_tester.py
# Compiled at: 2018-11-12 08:11:38
from __future__ import print_function
import subprocess as sp, operator as op

DEBUG = False
TIP = True
TOTAL_POINTS = 7
POINTS = 0
PASSED = 'Test PASSED: {}'
FAILED = 'Test FAILED: {}'
DATEFORMAT = '%a %b %d %H:%M:%S %Y'
PROG_NAME = './threadsched'
TESTS = [
    {'name': 'sample RR', 'n': 3,
     't': 10, 'q': 50, 'a': 'RR', 'threads': [{'prio': 1, 'id': 1, 'start': 100, 'rest_time': 200},
                                              {'prio': 1, 'id': 2, 'start': 200, 'rest_time': 200},
                                              {'prio': 1, 'id': 3, 'start': 300, 'rest_time': 200}], 'pts': 0.5},
    {'name': 'sample PRR', 'n': 4,
     't': 10, 'q': 50, 'a': 'PRR', 'threads': [{'prio': 1, 'id': 1, 'start': 50, 'rest_time': 200},
                                               {'prio': 1, 'id': 2, 'start': 100, 'rest_time': 200},
                                               {'prio': 2, 'id': 3, 'start': 150, 'rest_time': 200},
                                               {'prio': 1, 'id': 4, 'start': 200, 'rest_time': 200}], 'pts': 0.5},
    {'name': 'sample SRTN', 'n': 3,
     't': 10, 'q': 50, 'a': 'SRTN', 'threads': [{'prio': 1, 'id': 1, 'start': 100, 'rest_time': 300},
                                                {'prio': 1, 'id': 2, 'start': 150, 'rest_time': 200},
                                                {'prio': 1, 'id': 3, 'start': 200, 'rest_time': 100}], 'pts': 0.5},
    # {'name': 'Value Range', 'n': 1,
    #   't': 10, 'q': 50, 'a': 'SRTN', 'threads': [{'prio': 1, 'id': 1, 'start': 100000, 'rest_time': 30000}], 'pts': 0.5},
    {'name': 'simple PRR case', 'n': 4,
     't': 10, 'q': 20, 'a': 'PRR',
     'threads': [{'prio': 3, 'id': 1, 'start': 0, 'rest_time': 50}, {'prio': 2, 'id': 2, 'start': 10, 'rest_time': 30},
                 {'prio': 1, 'id': 3, 'start': 20, 'rest_time': 80},
                 {'prio': 1, 'id': 4, 'start': 30, 'rest_time': 60}], 'pts': 1},
    {'name': 'simple RR case', 'n': 5,
     't': 5, 'q': 15, 'a': 'PRR',
     'threads': [{'prio': 1, 'id': 1, 'start': 0, 'rest_time': 80}, {'prio': 1, 'id': 2, 'start': 10, 'rest_time': 20},
                 {'prio': 1, 'id': 3, 'start': 10, 'rest_time': 10}, {'prio': 1, 'id': 4, 'start': 80, 'rest_time': 20},
                 {'prio': 1, 'id': 5, 'start': 85, 'rest_time': 50}], 'pts': 1},
    {'name': 'hard SRTN case', 'n': 10,
     't': 5, 'q': 15, 'a': 'SRTN',
     'threads': [{'prio': 1, 'id': 1, 'start': 9, 'rest_time': 9}, {'prio': 1, 'id': 2, 'start': 9, 'rest_time': 18},
                 {'prio': 1, 'id': 3, 'start': 9, 'rest_time': 20}, {'prio': 1, 'id': 4, 'start': 9, 'rest_time': 60},
                 {'prio': 1, 'id': 5, 'start': 25, 'rest_time': 10}, {'prio': 1, 'id': 6, 'start': 25, 'rest_time': 30},
                 {'prio': 1, 'id': 7, 'start': 25, 'rest_time': 20}, {'prio': 1, 'id': 8, 'start': 36, 'rest_time': 30},
                 {'prio': 1, 'id': 9, 'start': 36, 'rest_time': 40},
                 {'prio': 1, 'id': 10, 'start': 36, 'rest_time': 50}], 'pts': 1},
    {'name': 'hard RR case', 'n': 10,
     't': 10, 'q': 30, 'a': 'PRR',
     'threads': [{'prio': 1, 'id': 1, 'start': 0, 'rest_time': 40}, {'prio': 1, 'id': 2, 'start': 30, 'rest_time': 50},
                 {'prio': 1, 'id': 3, 'start': 30, 'rest_time': 60}, {'prio': 1, 'id': 4, 'start': 30, 'rest_time': 60},
                 {'prio': 1, 'id': 5, 'start': 40, 'rest_time': 10}, {'prio': 1, 'id': 6, 'start': 45, 'rest_time': 30},
                 {'prio': 1, 'id': 7, 'start': 55, 'rest_time': 20}, {'prio': 1, 'id': 8, 'start': 70, 'rest_time': 30},
                 {'prio': 1, 'id': 9, 'start': 120, 'rest_time': 40},
                 {'prio': 1, 'id': 10, 'start': 300, 'rest_time': 50}], 'pts': 1},
    {'name': 'hard PRR case', 'n': 10,
     't': 10, 'q': 20, 'a': 'PRR',
     'threads': [{'prio': 4, 'id': 1, 'start': 10, 'rest_time': 30}, {'prio': 4, 'id': 2, 'start': 10, 'rest_time': 40},
                 {'prio': 4, 'id': 3, 'start': 10, 'rest_time': 20}, {'prio': 3, 'id': 4, 'start': 10, 'rest_time': 60},
                 {'prio': 3, 'id': 5, 'start': 20, 'rest_time': 30}, {'prio': 3, 'id': 6, 'start': 20, 'rest_time': 30},
                 {'prio': 2, 'id': 7, 'start': 20, 'rest_time': 20}, {'prio': 2, 'id': 8, 'start': 40, 'rest_time': 30},
                 {'prio': 2, 'id': 9, 'start': 40, 'rest_time': 40},
                 {'prio': 1, 'id': 10, 'start': 150, 'rest_time': 50}], 'pts': 1}]


def build_cmd(test):
    cmd_format = PROG_NAME
    cmd_format += ' -n {test[n]} -t {test[t]} -q {test[q]} -a {test[a]}'
    return cmd_format.format(test=test)


def exec_test(test):
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, shell=True)
    for line in test['threads']:
        if DEBUG:
            print(line)
        threadline = '{thread[prio]} {thread[start]} {thread[rest_time]}\n'
        prog.stdin.write(threadline.format(thread=line))

    raw_output = prog.stdout.readlines()
    output = []
    for line in raw_output:
        output.append(line.decode('utf-8')[:-1])

    if DEBUG:
        for line in output:
            print(line)

    prog_runlist = check_format(test, output)
    if prog_runlist is None:
        return
    if DEBUG:
        print(prog_runlist)
    if test['a'] == 'RR':
        alg_test_func = sched_rr
    else:
        if test['a'] == 'PRR':
            alg_test_func = sched_prr
        else:
            if test['a'] == 'SRTN':
                alg_test_func = sched_srtn
    test_runlist = alg_test_func(test)
    if prog_runlist != test_runlist:
        if DEBUG or TIP:
            if len(prog_runlist) > len(test_runlist):
                print('wrong schedule: threads finished too late')
                print(('\texpected length: {}').format(len(test_runlist)))
                print(('\tgot length: {}').format(len(prog_runlist)))
            elif len(prog_runlist) < len(test_runlist):
                print('wrong schedule: threads finished too early')
                print(('\texpected length: {}').format(len(test_runlist)))
                print(('\tgot length: {}').format(len(prog_runlist)))
            else:
                for i in range(len(test_runlist)):
                    if prog_runlist[i] != test_runlist[i]:
                        print(('wrong schedule: mismatch at time {} (line {})').format(i * test['t'], i + 2))
                        print('expected: ' + str(test_runlist[:i + 1])[1:-1] + ', ...')
                        print('got:      ' + str(prog_runlist[:i + 1])[1:-1] + ', ...')
                        break

        fail_test(test)
        return
    pass_test(test)
    return


def check_format(test, output):
    """
    checks if the output has the right format
    (10 Slots, one Therad per line, table lines, "Time", last line non-empty
        not higher than thread n, timestep)
    :output: list of lines of output text
    :tq: (time_quantum)
    :n: number of threads
    
    :return: list of thread IDs running at (index*time_quantum) [0 = None]
             or None if the test failed
    """
    if output[0] != '  Time |  1  2  3  4  5  6  7  8  9  10':
        if DEBUG or TIP:
            print('wrong format: Header line')
        fail_test(test)
        return
    if output[1] != '-------+--------------------------------':
        if DEBUG or TIP:
            print('wrong format: Header seperator')
        fail_test(test)
        return
    runlist = []
    time_count = -test['t']
    for line in output[2:]:
        line_split = line.split('|')
        if len(line_split) != 2:
            if DEBUG or TIP:
                print('wrong format: time seperator')
            fail_test(test)
            return
        try:
            time = int(line_split[0])
        except ValueError as e:
            if DEBUG or TIP:
                print('wrong format: time missing')
            fail_test(test)
            return

        if time == time_count + test['t']:
            time_count = time
        else:
            if DEBUG or TIP:
                print('wrong format: wrong time step')
            fail_test(test)
            return
        try:
            tid = int(line_split[1])
        except ValueError as e:
            tid = 0

        if tid != 0:
            if line_split[1].find(str(tid)) != tid * 3 - 1:
                if DEBUG or TIP:
                    print('wrong format: tid not in right column')
                fail_test(test)
                return
        runlist.append(tid)

    return runlist


def sched_rr(test):
    remaining = test['threads'][:]
    ready_list = []
    running = None
    time = 0
    quantum_lock = test['q']
    runlist = []
    while True:
        if DEBUG:
            print('-----')
            print('remaining: ' + str(remaining))
            print('ready_list: ' + str(ready_list))
            print('running: ' + str(running))
            print('time: ' + str(time))
            print('quantum_lock: ' + str(quantum_lock))
        for thread in remaining[:]:
            if thread['start'] <= time:
                ready_list.append(thread)
                remaining.remove(thread)

        if running:
            running['rest_time'] -= test['t']
            quantum_lock -= test['t']
            if quantum_lock <= 0:
                quantum_lock = test['q']
                if running['rest_time'] <= 0:
                    running = None
                if ready_list:
                    if running:
                        ready_list.append(running)
                    running = ready_list.pop(0)
        else:
            if ready_list:
                running = ready_list.pop(0)
        if not running and not ready_list and not remaining:
            if DEBUG:
                print(runlist)
            return runlist
        runlist.append(running['id'] if running else 0)
        time += test['t']

    return


def sched_prr(test):
    remaining = test['threads'][:]
    ready_list = [[], [], [], [], [], [], [], [], [], [], []]
    running = None
    time = 0
    quantum_lock = test['q']
    runlist = []
    while True:
        if DEBUG:
            print('-----')
            print('remaining: ' + str(remaining))
            print('ready_list: ' + str(ready_list))
            print('running: ' + str(running))
            print('time: ' + str(time))
            print('quantum_lock: ' + str(quantum_lock))
        for thread in remaining[:]:
            if thread['start'] <= time:
                ready_list[thread['prio']].append(thread)
                remaining.remove(thread)

        if running:
            running['rest_time'] -= test['t']
            quantum_lock -= test['t']
            if quantum_lock <= 0:
                quantum_lock = test['q']
                if running['rest_time'] <= 0:
                    running = None
                if ready_list:
                    if running:
                        ready_list[running['prio']].append(running)
                    for prio_ready_list in ready_list:
                        if prio_ready_list:
                            running = prio_ready_list.pop(0)
                            break

        else:
            for prio_ready_list in ready_list:
                if prio_ready_list:
                    running = prio_ready_list.pop(0)
                    break

        ready_list_empty = True
        for prio_ready_list in ready_list:
            if prio_ready_list:
                ready_list_empty = False

        if not running and ready_list_empty and not remaining:
            if DEBUG:
                print(runlist)
            return runlist
        runlist.append(running['id'] if running else 0)
        time += test['t']

    return


def sched_srtn(test):
    remaining = test['threads'][:]
    ready_list = []
    running = None
    time = 0
    quantum_lock = test['q']
    runlist = []
    while True:
        if DEBUG:
            print('-----')
            print('remaining: ' + str(remaining))
            print('ready_list: ' + str(ready_list))
            print('running: ' + str(running))
            print('time: ' + str(time))
            print('quantum_lock: ' + str(quantum_lock))
        for thread in remaining[:]:
            if thread['start'] <= time:
                ready_list.append(thread)
                ready_list.sort(key=op.itemgetter('rest_time'))
                remaining.remove(thread)

        if running:
            running['rest_time'] -= test['t']
            quantum_lock -= test['t']
            if quantum_lock <= 0:
                quantum_lock = test['q']
                if running['rest_time'] <= 0:
                    running = None
                if ready_list:
                    if running:
                        ready_list.append(running)
                        ready_list.sort(key=op.itemgetter('rest_time'))
                    running = ready_list.pop(0)
        else:
            if ready_list:
                running = ready_list.pop(0)
        if not running and not ready_list and not remaining:
            if DEBUG:
                print(runlist)
            return runlist
        runlist.append(running['id'] if running else 0)
        time += test['t']

    return


def fail_test(test):
    print(FAILED.format(test['name']))


def pass_test(test):
    global POINTS
    print(PASSED.format(test['name']))
    POINTS += test['pts']


def main():
    for test in TESTS:
        exec_test(test)

    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()
# okay decompiling threadsched_tester.pyc
