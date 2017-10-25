#!/usr/bin/env python

## Commentary:
'''
What this command does is simple but obfuscated by the syntax.
If you know lisp, its a lot like continuation passing style.
If you don't know lisp, this function is a lot like a function that takes a callback.
If you don't know about either, no worries.
A continuation is a thing that represents control of the program.
A continuation can be passed as an argument to a function or used as the return value of a function.
While the definition itself sounds like gobbldygook, it is easy to read and write examples
of continuations.
Consider this C-like psuedo code, where `anything` is a type that could be litteraly anything

anything takes_continuation(function myfunction) {
/* Compute a value and store it in x */
int x = 2 + 2;
return myfunction(x);
}

As you can see, this function is open ended. Calling it would look like:

takes-continuation(printf);

which would print 4.
But lets say you used a function that did more with the x value:

int do-stuff-with-x (int x) {
return x + 4;
}

So now, if you call takes-continuation(do-stuff-with-x), you get (2 + 2) = 4, 4 + 4 = 8.
Note that 2 + 2 is evaluated before 4 + 4. A continuation results in the order being turned inside out:
while normally the innermost function is executed first, when a function is taking a continuation it
executes before what comes inside of it. This is because you are not passing do-stuff-with-x(4) to
to takes-continuation, but instead _control_ of the program is passed from
takes_continuation to do-stuff-with-x. This can be illustrated with a working python example:

def takes_continuation(n, k): # where k is an abbreviation for continuation
    # Calls continuation k on the result of adding n and 6
    y = n + 6
    return k(y)

def receives_control_state(n): # floors x at 7
    if n > 7:
        return n
    else:
        return 8

def also_receives_control_state(n): # caps x at 7
    if n < 7:
        return n
    else:
        return 6

Now at the prompt:

>>> x = takes_continuation(10, receives_control_state)
>>> x
16
>>> x = takes_continuation(10, also_receives_control_state)
>>> x
6
   
As you can see, receives_control_state is explicitly next in line in the control flow of
the program: after the calculation, n + 6, is done by `takes_continuation`, k, or in
this case, receives_control_state, gets the control, as it gets to decide where the program will go.
Similarly, when also_receives_control_state is passed to `takes_continuation` as an argument,
it also receives the control flow.
In other words, the _control flow_ of the program becomes an _explicit, modifiable parameter_.

So now you may be saying to yourself, "Ok, continuations are cool and all, but how is this relevant?"
Hold on, dear reader. To answer that question, we need more examples and explanation (yay).
In making these tests, you need some way to handle the lifetime of svrd such that
1. svrd is called before the process that requires svrd
2. svrd opens a source with the type v4l before the process that requires svrd is opened
3. the svrd process is killed at the end of the test (to avoid messing with repeat tests)
4. the script exits with the return value of the process that requires svrd
Imagine trying to implement this in a completely procedural language.
It is easy to see a solution like (bash like psuedocode):

svrd;
if v4l is opened, then execute the process, endif;
kill svrd;
exit with process' return code;

There's only one problem with this. You can't know v4l is opened without reading from the debug info.
So to resolve the issue, one must read from the stream. Returning to the psuedocode, we have:

svrd -d | svrd-stream-reader;
execute the process;

Oh noes! Now svrd-stream-reader can't exit before svrd! `execute the process;` won't get
called until `svrd -d | svrd-stream-reader;` returns.
So now we have a problem: in super-procedural bash, which is evaluated instruction-by-instruction,
we want to go to the next instruction...
before the next instruction is available. This is where the continuation comes in:

svrd -d | svrd-stream-reader-takes-continuation continuation;

Now svrd-stream-reader-takes-continuation can wait for the proper conditions to be met, call
the function "continuation", and return what the function "continuation" returns.
Which is exactly what this script does. Of course, calling it looks a little different from

svrd -d | svrd-stream-reader-takes-continuation continuation;

It looks a lot more like

start-svrd-continuation <path-to-svrd-binary> <continuation>

where continuation is a program and its args, such as

  program                   args
   /--\       /-------------------------------\
   bash       normalscript.sh --option argument

or

svrctl --help
'''


# Call svrd, wait for v4l to be opened,
# then start the executable passed to it as an arg.
# Finally, once that executable returns, kill svrd
# and exit with the status of the executable.
# Must be passed svrd as the first arg and the
# executable as the second
 
import sys
import subprocess

# Open the svrd process
svrd = subprocess.Popen([sys.argv[1], "-d"], stderr=subprocess.PIPE)
while True:
    # Read through the lines printed by svrd -d
    line = svrd.stderr.readline()
    # If the `[DEBUG][SVR] source_type 'v4l'` line is reached:
    if line.rstrip()[-4:][0:3] == 'v4l':
        # Then call the continuation, and store its return value in exit_status,
        exit_status = subprocess.call(sys.argv[2:])
        # do some cleanup,
        svrd.kill()
        # and finally return with exit_status.
        exit(exit_status)
