Using PHP 5 with the Zeus Web Server
-----------------------------------

Zeus fully supports running PHP in combination with our
webserver. There are three different interfaces that can be used to
enable PHP:

* CGI
* ISAPI
* FastCGI

Of the three, we recommend using FastCGI, which has been tested and
benchmarked as providing the best performance and reliability.

Full details of how to install PHP are available from our
website, at:

http://support.zeus.com/products/php.html

If you have any problems, please check the support site for more
up-to-date information and advice.


Quick guide to installing CGI/FastCGI with Zeus
-----------------------------------------------

Step 1 - Compile PHP as FastCGI.

Compile as follows:
        ./configure --enable-fastcgi
        make

Note that PHP has many options to the configure script -
e.g. --with-mysql. You will probably want to select your usual options
before compiling; the above is just a bare minimum, for illustration.

After compilation finishes, you will be left with an executable
program called 'php'. Copy this into your document root, under a
dedicated FastCGI directory (e.g. $DOCROOT/fcgi-bin/php)


Step 2 - configure Zeus

Four stages:
        -  enable FastCGI
        -  configure FastCGI
        -  setup alias for FastCGI
        -  setup alias for PHP

1) Using the admin server, go to the 'module configuration' page for
your virtual server, and ensure that 'fastcgi' is enabled (select the
tickbox to the left).

2) While we can run FastCGI's locally, there are known problems with
some OS's (specifically, the communication between web server and
FastCGI happens over a unix domain socket, and some OS's have trouble
sustaining high connection rates over these sockets). So instead, we
are going to set up the PHP FastCGI to run 'remotely' over localhost
(this uses TCP sockets, which do not suffer this problem). Go to the
'fastcgi configuration' page, and under 'add remote fastcgi':
        Add Remote FastCGI
                Docroot path            /fcgi-bin/php
                Remote machine          localhost:8002
The first entry is where you saved PHP, above.
The second entry is localhost:<any unused port>
We will start the FastCGI listening on this port shortly.
Click 'update' to commit these changes.

3) Go to the path mapping module and add an alias for FastCGI:
        Add Alias
                Docroot path            /fcgi-bin
                Filesystem directory    /path/to/docroot/fcgi-bin
                Alias type              fastcgi
Click 'update' to commit these changes

4) Also on the path mapping module, add a handler for PHP:
        Add handler
                File extension          php
                Handler                 /fcgi-bin/php
Click 'update' to commit these changes

Finally restart your virtual server for these changes to take effect.


Step 3 - start PHP as a FastCGI runner

When you start PHP, it will pre-fork a given number of child processes
to handle incoming PHP requests. Each process will handle a given
number of requests before exiting (and being replaced by a newly
forked process). You can control these two parameters by setting the
following environment variables BEFORE starting the FastCGI runner:

PHP_FCGI_CHILDREN - the number of child processes to pre-fork. This
variable MUST be set, if not then the PHP will not run as a FastCGI.
We recommend a value of 8 for a fairly busy site. If you have many,
long-running PHP scripts, then you may need to increase this further.

PHP_FCGI_MAX_REQUESTS - the number of requests each PHP child process
handles before exiting. If not set, defaults to 500.

To start the FastCGI runner, execute '$ZEUSHOME/web/bin/fcgirunner
8002 $DOCROOT/fcgi-bin/php'.  Substitute the appropriate values for
$ZEUSHOME and $DOCROOT; also substitute for 8002 the port you chose,
above.

To stop the runner (e.g. to experiment with the above environment
variables) you will need to manually stop and running PHP
processes. (Use 'ps' and 'kill'). As it is PHP which is forking lots
of children and not the runner, Zeus unfortunately cannot keep track
of what processes are running, sorry. A typical command line may look
like 'ps -efl | grep $DOCROOT/fcgi-bin/php | grep -v grep | awk
'{print $4}' | xargs kill'
