# mini-project-1-template

# Running the shell
Run the follwing commands
1) make clean
2) make
3) ./shell

# Assumptioms

Warnings generated on compiling can be ignored

If the input contains only 1 command which is errorneous it is not added in the logs. But if there are multiple commands out of which 1 is erroneous the entire line is added in the log

Initialize previous directory to the home directory initially i.e. there will always be a previous directory

In some areas of code time.h library gives error in VSCODE but compiles fine whereas linux/time.h works correctly and is used there

Size of virtual memory in Spec 7 is measured in KB

If we execute "sleep 3 ; sleep 5", first sleep 3 is executed and then sleep 5 gets executed, but the next prompt will show <text sleep : 5s>

Backgrounding of custom commands is not done

If we have already reached the topmost directory after executing hop .. many times, then further cases of hop .. will output "Already reached topmost directory"

The command "hop" w/o any other argument is equivalent to performing hop HOME_DIR

While performing reveal -l, color coding is performed on the entire line

Seek -e requires either -f or -d flag to work 

Commands of the form "part1 part2 & part3" will be considered as "part1 part2 &"

When an alias command is run, the original command is not added in the input but the alias is added

In activities we have to add processes spawned by the shell, thus custom implemented commands will not appear. Also since we cannot run activities when a foreground process is currently running, we need to implement activities only for background processes

Ctrl D command will not work when a foreground process is in execution. To interupt a foreground process Ctrl C must be used after which Ctrl D can be used

Grep command in pipes doesnt work if we use "" around the text to be searched i.e. ls | grep "a" will not work but instead ls | grep a works

I am handling the case that an alias is created using another alias

Comments in .myshrc start from #. Everything in any given line is ignored once you encounter # in that line

I have assumed that any alias will not be referencing to a background command
Alias doesnt work as background command. Should the alias command also work as a background process? [KM] Not required, but you're free to do so if you'd like.

If we start typing a command and then press ctrl-d, the command gets executed
We have to do another ctrl-d to exit from the terminal

Signal 19 is for SIGSTOP, 18 is for SIGCONT
Sending signals 9 (SIGKILL) and 15 (SIGTERM) terminates the process and on doing activities it wont be visible as stopped. Terminated processes are not "stopped", they simply are removed from your list of processes

what should we do in case of sleep < a.txt ; hop .. and a.txt contains 2 & sleep 3 ; hop .. . so should the command run like sleep 2 & sleep 3 ; hop .. ; hop ..?? [PS] The each sub command seprated by a ; executes independently of other, in the order they appear. Since sleep < a.txt regardless of contents of a.txt would throw a error, since input redirection is not equivalent to function paramaters, thus provided example's output would be an error follwed by change in directory and corresponding output.    The behaviour of linux shell is different. It outputs sleep: missing parameter and doesnt execute the further commands

There might be errors because of escape sequence artifacts (used in color coding) in txt files

In the case of redirection, error_command > a.txt will print the error in a.txt

If a process gives error it is not added in the activities list

The max lenght of everything is assumed to be 4096

emacs command behaves strangely

If fg/bg <id> is used then fg/bg will get stored in the log and not the command corresponding to that

Refrain from using commands like "cd <text>" and "vim &". They give unexpected outcomes
