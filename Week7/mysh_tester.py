# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: mysh_tester.py
# Compiled at: 2018-12-03 11:17:32
from __future__ import print_function
import subprocess as sp, random as r, os
DEBUG = False
POINTS = 0.0
TOTAL_POINTS = 10
PROG_NAME = './mysh'
PASSED = 'Test PASSED: {} [{}/{} pts]'
FAILED = 'Test FAILED: {} [{}/{} pts]'
infile_random_num = 0

def io_test_read(test):
    global infile_random_num
    if not create_infile():
        fail_test(test)
        return
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    prog_input = 'cat < infile.txt\nexit\n'
    output, err = prog.communicate(prog_input)
    if prog.returncode == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if prog.returncode == 127:
        print(PROG_NAME + ' not found')
        fail_test(test)
        return
    if err:
        print('stderr:')
        print(err)
        fail_test(test)
        return
    if DEBUG:
        print('irn: ' + str(infile_random_num))
        print('output: ' + output)
    if str(infile_random_num) in output:
        pass_test(test)
    else:
        print('Did not read the input file correctly.')
        fail_test(test)
        return


def io_test_write(test):
    if not remove_outfile():
        fail_test(test)
        return
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    random_num = r.randint(0, 2147483648)
    prog_input = 'echo ' + str(random_num) + ' > outfile.txt\nexit\n'
    output, err = prog.communicate(prog_input)
    if prog.returncode == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if prog.returncode == 127:
        print(PROG_NAME + ' not found')
        fail_test(test)
        return
    if err:
        print('stderr:')
        print(err)
        fail_test(test)
        return
    cwd = os.path.dirname(os.path.realpath(__file__))
    try:
        outfile = open(cwd + '/' + 'outfile.txt', 'r')
    except Exception as e:
        print("Could not open outfile.txt. Probably doesn't exist")
        fail_test(test)
        return

    outfile_content = outfile.readline()
    if DEBUG:
        print('rn: ' + str(random_num))
        print('outfile_content: ' + outfile_content)
    if str(random_num) in outfile_content:
        pass_test(test)
    else:
        print('Did not write the output file correctly.')
        fail_test(test)


def io_test_read_write(test):
    if not create_infile():
        fail_test(test)
        return
    if not remove_outfile():
        fail_test(test)
        return
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    prog_input = 'cat < infile.txt > outfile.txt\nexit\n'
    output, err = prog.communicate(prog_input)
    if prog.returncode == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if prog.returncode == 127:
        print(PROG_NAME + ' not found')
        fail_test(test)
        return
    if err:
        print('stderr:')
        print(err)
        fail_test(test)
        return
    cwd = os.path.dirname(os.path.realpath(__file__))
    try:
        outfile = open(cwd + '/' + 'outfile.txt', 'r')
    except Exception as e:
        print("Could not open outfile.txt. Probably doesn't exist")
        fail_test(test)
        return

    outfile_content = outfile.readline()
    if DEBUG:
        print('irn: ' + str(infile_random_num))
        print('outfile_content: ' + outfile_content)
    if str(infile_random_num) in outfile_content:
        pass_test(test)
    else:
        print('Did not read the input file correctly.')
        fail_test(test)


def pipe_test(test):
    prog = sp.Popen(build_cmd(test), stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE, shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
    output, err = prog.communicate(test['input'])
    if prog.returncode == 124:
        print('Program timed out, did not termitate in time')
        fail_test(test)
        return
    if prog.returncode == 127:
        print(PROG_NAME + ' not found')
        fail_test(test)
        return
    if err:
        print('stderr:')
        print(err)
        fail_test(test)
        return
    if DEBUG:
        print(output)
    for oc in test['output_contains']:
        if oc not in output:
            print('missing from output: ' + oc)
            fail_test(test)
            return

    pass_test(test)


TESTS = [
 {'name': 'I/O-Redirect: writing from file', 'timeout': 5, 
    'test_func': io_test_write, 
    'pts': 2},
 {'name': 'I/O-Redirect: reading from file', 'timeout': 5, 
    'test_func': io_test_read, 
    'pts': 2},
 {'name': 'I/O-Redirect: reading and writing from file', 'timeout': 5, 
    'test_func': io_test_read_write, 
    'pts': 3},
 {'name': 'Pipe: case 1', 'timeout': 5, 
    'test_func': pipe_test, 
    'input': 'echo 0123456789 | wc -c\nexit\n', 
    'output_contains': [
                      '11'], 
    'pts': 1},
 {'name': 'Pipe: case 2', 'timeout': 5, 
    'test_func': pipe_test, 
    'input': 'echo Hallo Welt | rev \nexit\n', 
    'output_contains': [
                      'tleW ollaH'], 
    'pts': 1},
 {'name': 'Pipe: case 3', 'timeout': 5, 
    'test_func': pipe_test, 
    'input': 'echo GBS TEST BLATT 7 | grep GBS\nexit\n', 
    'output_contains': [
                      'GBS TEST BLATT 7'], 
    'pts': 1}]

def build_cmd(test):
    cmd = 'timeout ' + str(test['timeout']) + ' ' + PROG_NAME
    if DEBUG:
        print('cmd: ' + cmd)
    return cmd


def create_infile():
    """
    creates a file for reading from it. If the file already exists, do nothing
    :return: False on Error, True on sucess
    """
    global infile_random_num
    cwd = os.path.dirname(os.path.realpath(__file__))
    infile_path = cwd + '/' + 'infile.txt'
    if os.path.exists(infile_path):
        if not os.path.isfile(infile_path):
            print('Critical Error: infile.txt already exists, and is notremoveable. Please remove it! Aborting Test Case')
            return False
        os.remove(infile_path)
    infile = open(infile_path, 'w')
    infile_random_num = r.randint(0, 2147483648)
    infile.write(str(infile_random_num) + '\n')
    infile.close()
    return True


def remove_outfile():
    cwd = os.path.dirname(os.path.realpath(__file__))
    outfile_path = cwd + '/' + 'outfile.txt'
    if os.path.exists(outfile_path):
        if not os.path.isfile(outfile_path):
            print('Critical Error: outfile.txt already exists, and is notremoveable. Please remove it! Aborting Test Case')
            return False
        os.remove(outfile_path)
    return True


def remove_infile():
    cwd = os.path.dirname(os.path.realpath(__file__))
    infile_path = cwd + '/' + 'infile.txt'
    if os.path.exists(infile_path):
        if not os.path.isfile(infile_path):
            print('Critical Error: infile.txt already exists, and is notremoveable. Please remove it! Aborting Test Case')
            return False
        os.remove(infile_path)
    return True


def clean_up():
    remove_outfile()
    remove_infile()


def fail_test(test):
    print(FAILED.format(test['name'], 0, test['pts']))


def pass_test(test):
    global POINTS
    POINTS += test['pts']
    print(PASSED.format(test['name'], test['pts'], test['pts']))


def main():
    print(os.path.basename(os.path.realpath(__file__)) + ':')
    for test in TESTS:
        try:
            test['test_func'](test)
        except Exception as e:
            print(e)

    clean_up()
    print(('Points received: {0}/{1}').format(POINTS, TOTAL_POINTS))


if __name__ == '__main__':
    main()