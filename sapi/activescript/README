This is the ActiveScript SAPI for PHP.
======================================

Once registered on your system (using regsvr32), you will be able to use
PHP script in any ActiveScript compliant host.  The list includes:

o. Windows Script Host
o. ASP and ASP.NET
o. Windows Script Components / Behaviours
o. MS Scriptlet control

Probably the most useful of these will be using it with the scriptlet
control, or in your own activescript host, so that you can very easily
embed PHP into your win32 application.

Installation.
=============

Build and install it somewhere; then register the engine like this:

  regsvr32 php5activescript.dll

Configuration.
==============

PHPScript will not use the default php.ini file.
Instead, it will look only in the same directory as the .exe that caused it to
load.

You should create php-activescript.ini and place it in that folder, if you wish
to load extensions etc.

Usage.
======

o. Windows Script Host

  Create a .wsf file like this:

  <job id="test">
    <script language="PHPScript">
	  $WScript->Echo("Hello");
	</script>
  </job>

o. ASP and ASP.NET

  <%@language=PHPScript %>
  <% $Response->Write("Hello"); %>

o. Windows Script Components / Behaviours

  Use language="PHPScript" on your <script> tags

o. MS Scriptlet control

  Set the language property to "PHPScript"


