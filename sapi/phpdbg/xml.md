phpdbg XML format
=================

Common attributes
=================

severity
--------

- indicates the genre of phpdbg system output
- usually one of these values:
 - normal
 - notice
 - error

msgout
------

- text message output related to the xml data (e.g. &lt;intro severity="normal" help="help" msgout="To get help using phpdbg type &amp;quot;help&amp;quot; and press enter" />)

req
---

- the request id, if one was passed to the last command (via -r %d, where %d is the id) (and the output is related to that message)

file
----

- refers to a filename

method
------

- format classname::methodname
- refers to a method

function
--------

- refers to a function

symbol
------

- either function or method (is method if "::" are present)

opline
------

- in hexadecimal format
- refers to a specific pointer to a (zend_)op

opcode
------

- refers to an opcode (ZEND_*)

type
----

- general attribute for most errors, describes the genre of the error

General tags
============

intro
-----

- appears on startup if -q flag wasn't provided as command line arg
- before any input possibility
- attributes may be spread over multiple tags
- wrapped in &lt;intros> tag

### attributes ###

- version: current phpdbg version (as string)
- help: command name for help
- report: URL for bug reporting


phpdbg
------

- general text message output from phpdbg system

stream
------

- any output by PHP itself (e.g. &lt;stream type="stdout">test&lt;/stream>)

### attributes ###

- type: stderr or stdout

php
---

- php error output

### attributes ###

- msg: the error message


General error tags
==================

command
-------

- general errors about commands

### possible attributes ###

- type
 - toomanyargs: more arguments than allowed
 - noarg: argument missing
 - wrongarg: wrong type of argument (e.g. letters instead of integer)
 - toofewargs: not enough arguments
 - notfound: command (or subcommand) doesn't exist
 - ambiguous: command was ambiguous
 - invalidcommand: command input is totally invalid
 - (nostack: should not happen: is an internal error)
 - (emptystack: should not happen: is an internal error)
- command: passed command
- subcommand: passed subcommand (present if the error is related to the subcommand)
- expected: count of expected arguments
- got: type of argument for type "wrongarg"
- num: if possible, information about which parameter had a wrong argument

inactive
--------

- by type
 - op_array: nothing was yet compiled (probably because no execution context set)
 - symbol_table: no symbol table present (not yet initiailized or already destructed)
 - noexec: not in execution
 - memory_manager: using the native memory manager (malloc, free, realloc) instead of e.g. the Zend MM
 - notfound: file not found
 - nocontext: execution context was not set (or compilation had failed)
 - isrunning: command requires no running script


Commands
========

export
------

- tag: &lt;exportbreakpoint />
- usually triggered by successful export command
- may appear when cleaning to temporary store breakpoints
- errors by type
 - openfailure: could not create file

### attributes ###

- count: number of exported breakpoints

break / info break
------------------

- General tag for breakpoint creation, deletion and hits is "&lt;breakpoint />"

### possible attributes ###

- id: the breakpoint id (if the leave command was executed, the id has the value "leave")
- num: the nth opline of a function/method/file
- add: has value "success"/"fail": a brekpoint was successfully/not added
- pending: the breakpoint is waiting for resolving (e.g. a file opline on a not yet loaded file)
- deleted: has value "success"/"fail": a breakpoint was successfully/not deleted
- eval: the condition on conditional breakpoints
- file
- opline
- opcode
- symbol
- function
- method
- line


- listing breakpoints always in a container element "&lt;breakpoints>"
 - Child nodes:
  - function
  - method
  - file
  - opline
  - methodopline
  - functionopline
  - fileopline
  - evalfunction
  - evalfunctionopline
  - evalmethod
  - evalmethodopline
  - evalfile
  - evalopline
  - eval
  - opcode
 - attributes:
  - name: name of the symbol (function/method/file/opcode)
  - disabled: empty value if enabled, non-empty if enabled

- errors (by type)
 - exists: the breakpoint already exists
 - maxoplines: tries to break at an opline (usedoplinenum) higher than the number of present oplines (in maxoplinenum)
 - nomethod: method doesn't exist
 - internalfunction: one cannot break on an opline of an internal function
 - notregular: tries to set a breakpoint in not a regular file
 - (invalidparameter: should not happen: is an internal error)

frame
-----

- General tag for frames is "&lt;frame>"
- always has id attribute; if it only has id attribute, it just indicates current frame number, no other elements follow
- may contain other elements (of type &lt;arg>) when contained in &lt;backtrace> tag
- &lt;arg> always contains a &lt;stream> element, the value of the variable

### possible attributes ###

- id: the frame id, current frame has id 0 (frames with internal function calls have the same id than their called frame)
- symbol ("{main}" is root frame)
- file
- line
- internal: has value "internal" when being an internal function call (one cannot inspect that frame)

- being an error: (by type)
 - maxnum: tried to access a frame with a number higher than existing (or &lt; 0)

### attributes on &lt;arg> ###

- variadic: has a non-empty value if the argument is variadic
- name: variable name of parameter

info (subcommands)
------------------

### break ###

- See above ("break / info break")

### files ###

- lists included files
- &lt;includedfileinfo num="" /> with num having an integer value, indicating the number of included files
- &lt;includedfile name=""/>: one per file, with name being the file path of the included file

### error ###

- gets last error
- &lt;lasterror error="" (file="" line="") />
- error attribute contains the last error as a string, is empty if there's no last error

### vars / globals ###

- &lt;variableinfo num="" /> with num having an integer value, indicating the number of (local or superglobal) variables
- if info vars was used it'll have also one of these attributes:
 - method
 - function
 - file
 - opline
- for each variable there is a &lt;variable> followed by a &lt;variabledetails> element
- &lt;variable address="" refcount="" type="" name="" />
 - address: pointer to zval (hexadecimal)
 - refcount: refcount of zval
 - type: the variable type (long, string, ...). If the value is "unknown", the other attributes are meaningless
 - name: the name of the variable
 - refstatus: empty if the zval is not a reference
 - class: the class the object in the zval is an instance of
 - resource: the type of the resource in the zval

### literal ###

- &lt;literalinfo num="" /> with num having an integer value, indicating the number of literals, optional arguments are:
 - method
 - function
 - file
 - opline
- for each literal there is a &lt;literal> followed by a &lt;stream type="stdout"> which prints the value of the literal
- &lt;literal id="" />: where id is the internal identifier of the literal

### memory ###

- Format:

    &lt;meminfo />
    &lt;current />
    &lt;used mem="" />
    &lt;real mem="" />
    &lt;peak />
    &lt;used mem="" />
    &lt;real mem="" />

- mem is an attribute whose value is a float. The memory is given in kilobytes (1 kB == 1024 bytes)

### classes ###

- &lt;classinfo num="" /> with num having an integer value, indicating the number of loaded user-defined classes
- Each class is enumerated with first a &lt;class>, then an optional &lt;parents> container and then a &lt;classsource> element
- The &lt;parents> container contains the &lt;class> elements of the parent of the last &lt;class> element.
- &lt;class type="" flags="" name="" methodcount="" />
 - type: either "User" or "Internal"
 - flags: either "Interface", "Class" or "Abstract Class"
- &lt;classsource /> where the class was defined, if there are no attributes, location is unknown, usually defined by
 - file
 - line

### funcs ###

- &lt;functioninfo num="" /> with num having an integer value, indicating the number of loaded user-defined functions
- Each class is enumerated with first a &lt;function> and then a &lt;functionsource> element
- &lt;function name="" />
- &lt;functionsource /> where the function was defined, if there are no attributes, location is unknown, usually defined by
 - file
 - line

list
----

- consists of &lt;line> elements wrapped in a &lt;list> container
- &lt;list file=""> is the container element with file being the filename
- &lt;line line="" code="" /> with value of code being the whole line of code in the line specified in the line attribute
 - current: this attribute is set to "current" if that line is the line where the executor currently is

print
-----

### without a subcommand ###

- &lt;print> elements are wrapped in a &lt;printinfo> element
- there may be a variable number of &lt;print> elements with a variable count of args inside the &lt;printinfo> element
- possible args are:
 - readline: yes/no - readline enabled or disabled
 - libedit: yes/no - libedit enabled or disabled
 - context: current executing context
 - compiled: yes/no - are there actual compiled ops?
 - stepping: @@ TODO (meaningless for now) @@
 - quiet: on/off - should it always print the opline being currently executed?
 - oplog: on/off - are oplines logged in a file?
 - ops: number of opcodes in current executing context
 - vars: number of compiled variables (CV)
 - executing: yes/no - in executor?
 - vmret: the return value of the last executed opcode
  - default: continue
  - 1: return from vm
  - 2: enter stack frame
  - 3: leave stack frame
 - classes: number of classes
 - functions: number of functions
 - constants: number of constants
 - includes: number of included files

### with a subcommand ###

- introduced by &lt;printinfo num="" /> (except for print opline) with num being the number of opcodes and one of these args:
 - file
 - method
 - function
 - class (then also type and flags attributes, see info classes command for their meanings)
 - symbol (also type and flags attributes; here the value of flags is either "Method" or "Function")
- if there is a class method, the methods are all wrapped in a &lt;printmethods> container
- then comes a &lt;printoplineinfo type="" /> where type is either "User" or "Internal"
- the &lt;printoplineinfo> has either a method or a function attribute
- if the type is "Internal"
 - there are no oplines, it's an internal method or function
- if the type is "User"
 - it has these attributes
   - startline: the first line of code where the method or function is defined
   - endline: the lastt line of code where the method or function is defined
   - file: the file of code where the method or function is defined
 - is followed by the oplines of that method or function (&lt;print> elements)
- &lt;print line="%u" opline="%p" opcode="%s" op="%s" />
- in case of print opline it emits a single &lt;opline line="" opline="" opcode="" op="" file="" />

exec
----

- command executing and compiling a given file
 - &lt;exec type="unset" context="" />: indicates unsetting of the old context
 - &lt;exec type="unsetops" />: indicates unsetting of the old compiled opcodes
 - &lt;exec type="unchanged" />: same execution context chosen again
 - &lt;exec type="set" context="" />: indicates setting of the new context
- errors by tag
 - &lt;compile>
  - openfailure: couldn't open file
  - compilefailure: The file indicated in context couldn't be compiled
 - &lt;exec>
  - invalid: given context (attribute) is not matching a valid file or symlink
  - notfound: given context (attribute) does not exist

run / &lt;stop> tag
-------------------

- runs the script (set via exec command)
- &lt;stop type="end" />: script execution ended normally
- (error) &lt;stop type="bailout" /> the VM bailed out (usually because there was some error)
- compile failures see under exec, errors, &lt;compile>

step
----

- steps by one line or opcode (as defined via set stepping) default is one line
- returns back to the executor

continue
--------

- returns back to the executor

until
-----

- temporarily disables all the breakpoints on that line until that line was left once
- returns back to the executor

finish
------

- temporarily disables all the breakpoints until the end of the current frame
- returns back to the executor

leave
------

- temporarily disables all the breakpoints past the end of the current frame and then stops
- returns back to the executor

back
----

- prints backtrace
- see frame command

ev
--

- eval()uates some code
- output wrapped in &lt;eval> tags

sh
--

- executes shell command
- still pipes to stdout ... without wrapping &lt;stream> !!! (@@ TODO @@)

source
------

- executes a file in .phpdbginit format
- errors by type
 - notfound: file not found

register
--------

- registers a function to be used like a command
- &lt;register function="" />: successfully registered function
- errors by type
 - notfound: no such function
 - inuse: function already registered

quit
----

- quits phpdbg
- if successful connection will be closed...

clean
-----

- cleans environment (basically a shutdown + new startup)
- &lt;clean> tags wrapped in a &lt;cleaninfo> container
- possible attributes of &lt;clean> tag
 - classes: number of classes
 - functions: number of functions
 - constants: number of constants
 - includes: number of included files

clear
-----

- removes all breakpoints
- &lt;clear> tags wrapped in a &lt;clearinfo> container
- possible attributes of &lt;clear> tag (value is always the number of defined breakpoints of that type)
 - files
 - functions
 - methods
 - oplines
 - fileoplines
 - functionoplines
 - methodoplines
 - eval

watch
-----

- watchpoints generally are identified by a variable (one may need to switch frames first...)
- &lt;watch variable="" />, &lt;watchrecursive variable="" /> and &lt;watcharray variable="" /> (normal, array, recursive)
- &lt;watch> if error, by type:
 - undefined: tried to set a watchpoint on a not (yet) defined variable
 - notiterable: element which is tried to be accessed as an object or array is nor array nor object
 - invalidinput: generally malformed input
- &lt;watchdelete variable="" />: when "watch delete" was used on a watchpoint
- (error) &lt;watchdelete type="nowatch" />: that watchpoint doesn't exist, so couldn't be deleted
- for hit watchpoints etc., see Other tags, &lt;watch*>
- when using watch list, &lt;watchvariable> elements are wrapped in a &lt;watchlist> container
 - &lt;watchvariable variable="" on="" type="" />
  - variable: watched variable (may be a variable of another scope!)
  - on: values are array or variable, depending on what is watched
  - type: values are recursive or simple, depending on whether the watchpoint is checked recursively or not

set
---

- a general error is type="wrongargs" where a wrong argument was passed to a subcommand; tag is then &lt;set*>

### prompt ###

- without other args, a &lt;setpromt str="" /> tag is emitted where the value of the str attribue is the value of the prompt
- when there is another arg, the prompt is changed to that arg, no further xml answer

### break ###

- enables / disables a given breakpoint silently with no further xml answer
- if the boolean switch is omitted, it emits current state in a &lt;setbreak id="" active="" /> where active is on or off
- error with type="nobreak", when no breakpoint with the given id exists

### breaks ###

- generally enables / disables breakpoint functionality silently with no further xml answer
- if the boolean switch is omitted, it emits current state in a &lt;setbreaks active="" /> where active is on or off

### color ###

- sets the color on prompt, error or notices
- &lt;setcolor type="" color="" code="" />: code is the color code of color, type is either:
 - prompt
 - error
 - notice
- errors by type:
 - nocolor: color doesn't exist
 - invalidtype: type wasn't one of the three allowed types

### colors ###

- generally enables / disables colors silently with no further xml answer
- if the boolean switch is omitted, it emits current state in a &lt;setcolors active="" /> where active is on or off

### oplog ###

- sets oplog
- (error) &lt;setoplog type="openfailure" file="" /> when it couldn't open the passed file path
- &lt;setoplog type="closingold" /> is emitted when there was a previous open oplog (and a file is passed)
- if no further argument is passed, it emits current state in a &lt;setoplog active="" /> where active is on or off

### quiet ###

- generally enables / disables quietness silently with no further xml answer
- if the boolean switch is omitted, it emits current state in a &lt;setquiet active="" /> where active is on or off

### setpping ###

- sets stepping to either opcode or line (so a step command will either advance one op or one line)
- if no further argument is passed, it emits current state in a &lt;setoplog type="" /> where active is opcode or line

### refcount ###

- generally enables / disables showing of refcount in watchpoint breaks silently with no further xml answer
- if the boolean switch is omitted, it emits current state in a &lt;setrefcount active="" /> where active is on or off

wait
----

- internally executes exec, so exec will output first (if binding to socket worked)

### attributes ###

- import: has value "success"/"fail"
- missingmodule/missingextension: modules/extensions loaded in the target SAPI, but not in phpdbg

### errors (by type) ###

- nosocket: couldn't establish socket
- invaliddata: invalid JSON passed to socket

dl
--

- loads a module or Zend extension at a given path
- if a relative path is passed, it's relative to the extension_dir ini setting

### attributes ###

- extensiontype: "Zend extension" or "module"
- name: the extension name
- path: the path where it was loaded

### errors (by type) ###

- unsupported: dynamic extension loading is unsupported
- relpath: relative path given, but no extension_dir defined
- unknown: general error with internal DL_LOAD() (for message see msg attribute)
- wrongapi: wrong Zend engine version (apineeded / apiinstalled attributes give information about the API versions)
- wrongbuild: unmatched build versions (buildneeded / buildinstalled attributes give information about the build versions)
- registerfailure: registering module failed
- startupfailure: couldn't startup Zend extension / module
- initfailure: couldn't initialize module
- nophpso: passed shared object is not a valid Zend extension nor module

- errors may have the module or extension attribute when their name is already known at the point of failure

Other tags
==========

&lt;signal>
-----------

- received caught signal

### attributes ###

- type: type of signal (e.g. SIGINT)

### by type ###

- SIGINT: interactive mode is entered...

&lt;watch*>
-----------

- generally emitted on hit watchpoint
- &lt;watchdelete variable="" />: when a variable was unset, the watchpoint is removed too
- &lt;watchhit variable="" />: when ever a watched variable is changed, followed by a &lt;watchdata> container
- &lt;watchdata> may contain
 - for watchpoints on variables:
  - each of these &lt;watch*> tags conatins a type attribute whose value is either "old" or "new")
  - &lt;watchvalue type="" inaccessible="inaccessible" />: old value is inaccessible
  - &lt;watchvalue type=""> may contain a &lt;stream> element which indicates the old/new (type attribute) value of the variable
  - &lt;watchrefcount type="" refcount="" isref="" />: old/new (type attribute) refcount and isref, both numbers
   - isref: if the value is 0, it's not a reference, else it is one
 - for watchpoints on arrays:
  - &lt;watchsize> inspects size variations of an array (the sum):
   - removed: number of elements removed
   - added: number of elements added
  - &lt;watcharrayptr>: if this tag appears, the internal pointer of the array way changed

&lt;signalsegv>
---------------

- generally emitted when data couldn't be fetched (e.g. by accessing inconsistent data); only used in hard interrupt mode
- it might mean that data couldn't be fetched at all, or that only incomplete data was fetched (e.g. when a fixed number of following attributes are fetched, this tag will mark a stop of fetching if none or not all tags were printed)
