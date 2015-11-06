lowculator: A lowb's calculator in C
-------------------------------------------------------------------------------
Use COMMANDLINE to start this program.
-------------------------------------------------------------------------------

COMMANDLINE:
    EXECUTABLE              (EXECUTABLE) (DEFUALT_COMMAND)
    EXECUTABLE COMMANDS     process all commands one by one.
COMMANDS:
    -c EXPRESSION           #precision EXPRESSION
    -e EXPRESSION           #evaluate EXPRESSION
    -f FILENAME             #file FILENAME
    -o FILENAME             #output EXPRESSION
    -p (on|off)             #p (on|off)
    -x EXPRESSION           #exit EXPRESSION
DEFUALT_COMMAND:
    "-f --"                 

EXECUTABLE:
    "lowculator"

ERROR_MESSAGE:
    SUCCESS:
        "Success."
    ERROR:
        "In (FILENAME), at (LINE):(COLUMN), ERROR_REASON."
ERROR_REASON:
    a string.               description of the error.

EXPRESSION:
    WS EXPRESSION
    ( EXPRESSION ) WS
    NUMBER
    EXPRESSION OPR EXPRESSION
    INVOCATION
NUMBER:
    BINARY_FLOATING
    OCTAL_FLOATING
    DECIMAL_FLOATING
    HEXADECIMAL_FLOATING
INTEGER:
    BINARY_INTEGER
    OCTAL_INTEGER
    DECIMAL_INTEGER
    HEXADECIMAL_INTEGER
DIGITS:
    BINARY_DIGITS:          [0-1]
    OCTAL_DIGITS:           [0-7]
    DECIMAL_DIGITS:         [0-9]
    HEXADECIMAL_DIGITS:     [0-9A-Fa-f]
HEADERS:
    BINARY_HEADER:
        "0b"
    OCTAL_HEADER:
        "0"
    DECIMAL_HEADER:
        ""
    HEXADECIMAL_HEADER:
        "0x"

X_FLOATING:         (X_INTEGER)(\.(X_DIGITS)+)?(\.?[Ee](INTEGER))?(WS)
X_INTEGER:          [\+\-]?(X_HEADER)(0|((X_DIGITS(except 0))(X_DIGITS)*))
OPR:                [\+\-\*\/\%\^](WS)
INVOCATION:
    FUNCTION:       (FUNCTION_NAME)(WS)\(EXPRESSION(,(EXPRESSION))?\)(WS)
    CONSTANT:       (CONSTANT_NAME)((WS)\((WS)\))?(WS)
WS:                 [\t ]*
X_NAME:             [A-Za-z][A-Za-z0-9_]*

FUNCTION_NAME:
    math functions.
CONSTANT_NAME:
    "pi"
    "e"

DIRECTIVE:                  #(DIRECTIVES)
DIRECTIVES:
    DIR_EOF                 eof
        =>End of file. That means, the current input file will be closed
            immediately (except stdin).

    DIR_EVALUATE:           evaluate (EXPRESSION)
        =>Evaluate an expression and print the result.

    DIR_EXIT:               exit (EXPRESSION)
        =>Exit program with the given exitcode (round to integer).

    DIR_FILE:               file (FILENAME)
        =>Process the given fie which contains EXPRESSIONs and DIRECTIVEs.

    DIR_PRECISION:          precision (EXPRESSION)
        =>Specify the precision (round to integer, non-negative and less
            than20) of the result to print. (default 8)

    DIR_OUTPUT:             output (FILENAME)
        =>Close the current output (except stdout), and print following output
            to the given file. (default stdout)

    DIR_PANIC:              panic (on|off)
        =>Turn on/off the panic mode. If turned on, any failed operation shall
            cause ther program exit. (default off)

FILENAME:
    "--"                    stdin or stdout.
    a string.               the path to a file.


