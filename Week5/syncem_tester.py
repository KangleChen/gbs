# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: ./syncem_tester.py
# Compiled at: 2018-11-19 10:17:54
import subprocess, sys
PROG_NAME = 'syncem'
POINTS = 6.0
NUM_THREADS = 10

def param_o(output, num_threads):
    received_points = param_f(output, num_threads)
    ordered = True
    if received_points > 3:
        lines = output.split('\n')[:-1]
        try:
            thread_ids = [ int(x[1:3]) for x in lines ]
        except:
            print 'Thread numbers could not be interpreted correctly.'
            return received_points

        done_threads = []
        previous = -1
        for t in thread_ids:
            if previous < 0:
                previous = t
            elif t != previous:
                if previous not in done_threads:
                    done_threads.append(previous)
                    previous = t
                else:
                    print 'Threads are interrupted by each other: %s, %s' % (previous, t)
                    ordered = False
                    break

        if ordered:
            done_threads.append(t)
            if done_threads == range(num_threads):
                received_points += 0.5
                return received_points
            print 'Order of threads incorrect.'
            return received_points


def param_f(output, num_threads):
    received_points = param_l(output, num_threads)
    no_interrupt = True
    if received_points > 2:
        lines = output.split('\n')[:-1]
        thread_ids = [ x[:4] for x in lines ]
        done_threads = []
        previous = -1
        for t in thread_ids:
            if previous < 0:
                previous = t
            elif t != previous:
                if previous not in done_threads:
                    done_threads.append(previous)
                    previous = t
                else:
                    print 'Threads are interrupted by each other: %s, %s' % (previous, t)
                    no_interrupt = False
                    break

        if no_interrupt:
            done_threads.append(t)
            received_points += 1.0
    return received_points


def param_l(output, num_threads):
    received_points = 0.0
    threads = range(num_threads)
    by_thread_id = {}
    for t in threads:
        by_thread_id[t] = []

    try:
        lines = [ x.split() for x in output.split('\n') ][:-1]
    except:
        print 'Output formatted incorrectly.'
        return 0.0

    formatting_ok = True
    len_ok = True
    rounds_ordered = True
    content_equal = True
    for line in lines:
        if len(line) != 3:
            print 'Line in output formatted incorrectly: %s' % line
            return 0.0

    for thread_id, round, out in lines:
        if len_ok and len(out) > 64:
            len_ok = False
            formatting_ok = False
            print 'Written output exceeds length of 64: %s' % out
            break
        try:
            if len(thread_id) == 4:
                thread_id = int(thread_id[1:-1])
            else:
                formatting_ok = False
                print 'Thread number is not formatted properly: %s' % thread_id
                break
        except:
            print 'Thread number could not be interpreted correctly: %s' % thread_id[1:-1]
            formatting_ok = False
            break

        if thread_id not in threads:
            print '-n not considered properly, too many threads.'
            formatting_ok = False
            break
        if len(round) > 3:
            print 'Round is not formatted properly: %s' % round
            formatting_ok = False
            break
        else:
            by_thread_id[thread_id].append((round, out))

    if formatting_ok:
        for key in by_thread_id.keys():
            if key not in threads:
                print '-n not considered properly, too many threads.'
                break
            try:
                rounds, outs = [ list(t) for t in zip(*by_thread_id[key]) ]
            except:
                continue

            try:
                rounds_int = map(int, rounds)
            except:
                print 'Round could not be interpreted correctly: %s' % round
                rounds_ordered = False
                formatting_ok = False
                break

            if rounds_ordered and rounds_int != range(rounds_int[-1] + 1):
                rounds_ordered = False
                print 'Order of rounds incorrect: %s' % rounds
            out_total = ('').join(outs)
            with open('%s.txt' % key) as (f):
                file_content = f.read()
                if content_equal and out_total != file_content:
                    content_equal = False
                    print 'Written output does not match file content completely: %s' % out_total
                else:
                    if content_equal and by_thread_id[key]:
                        threads.remove(key)

        if threads:
            print '-n not considered properly, missing output (partially) for following files: %s' % threads
        else:
            received_points += 0.5
        if len_ok:
            received_points += 0.5
        if rounds_ordered:
            received_points += 0.5
        if content_equal:
            received_points += 1.0
        return received_points
    return 0.0


def check_stdout(num_threads, param):
    cmd = './%s -n %d -%c' % (PROG_NAME, num_threads, param)
    try:
        output = subprocess.check_output(cmd, shell=True)
    except:
        print 'Could not check output of program.'
        return 0.0

    if not output:
        print 'No output was written.'
        return 0.0
    if param == 'l':
        return param_l(output, num_threads)
    if param == 'f':
        return param_f(output, num_threads)
    if param == 'o':
        return param_o(output, num_threads)


if __name__ == '__main__':
    try:
        syncem = open('%s.c' % PROG_NAME, 'r').read()
    except:
        pass

    if 'pthread_mutex' not in syncem:
        print 'Missing command: pthread_mutex(...)'
        sys.exit(1)
    points_received = 0.0
    points_received += check_stdout(NUM_THREADS, 'l')
    points_received += check_stdout(NUM_THREADS, 'f')
    print 'Points received: %0.2f/%0.2f' % (points_received, POINTS)