# 42sh

## GROUP MEMBERS

PARIS 20 <br>

david-ortiz.calderon <br>
maxime.francois-bardouil <br>
raj-sunil.mahajan-aka-mali <br>
terence.miralves <br>

## What is 42sh and how do I use it?

42sh is a replica of a POSIX Shell in C. It is able to parse and execute the
basics of what a POSIX Shell can.

## HOW TO MAKE 42sh

To build the project use the following commands:
```
make
```

To run tests to on 42sh and compare to the POSIX Shell
```
make check
```

## HOW TO USE 42sh

To execute shell code directly in 42sh
```
./42sh -c [shell code]
```

To execute a shell file
```
./42sh [shell file]
```

To execute a redirect
```
./42sh < [shell file]
```

## FEATURES TO IMPLEMENT
To print the AST (execution tree)
```
./42sh -print [shell code]
```
