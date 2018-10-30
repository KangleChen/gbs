# uncompyle6 version 3.2.4
# Python bytecode 2.7 (62211)
# Decompiled from: Python 2.7.13 (default, Sep 26 2018, 18:42:22) 
# [GCC 6.3.0 20170516]
# Embedded file name: shellsim_tester.py
# Compiled at: 2018-10-29 09:59:19
import os
from subprocess import Popen, PIPE
TOTAL_POINTS = 3
SRC = './shellsim.c'
CMD = './shellsim'

def main():
    points = 0
    shellsim = ''
    try:
        shellsim = open(SRC, 'r').read()
    except:
        pass

    if 'fgets' in shellsim and 'strncmp' in shellsim and 'system' in shellsim:
        print 'Test PASSED: correct commands used'
        points += 1
    else:
        print 'Test FAILED: wrong commands used'
    p = Popen(CMD, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    print('Opened')
    stdout, stderr = p.communicate('ls\nexit\n')
    print('Communicated')
    if os.path.basename(SRC) in stdout:
        print 'Test PASSED: ls command contains shellsim.c'
        points += 2
    else:
        print 'Test FAILED: ls command misses shellsim.c'
    print ('Points received: {0}/{1}').format(points, TOTAL_POINTS)


if __name__ == '__main__':
    main()