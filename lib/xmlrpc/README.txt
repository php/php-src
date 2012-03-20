= XMLRPC for Ruby, Standard Library Documentation

== Overview

XMLRPC is a lightweight protocol that enables remote procedure calls over
HTTP.  It is defined at http://www.xmlrpc.com.

XMLRPC allows you to create simple distributed computing solutions that span
computer languages.  Its distinctive feature is its simplicity compared to
other approaches like SOAP and CORBA.

The Ruby standard library package 'xmlrpc' enables you to create a server that
implements remote procedures and a client that calls them.  Very little code
is required to achieve either of these.

== Example

Try the following code.  It calls a standard demonstration remote procedure.

  require 'xmlrpc/client'
  require 'pp'

  server = XMLRPC::Client.new2("http://xmlrpc-c.sourceforge.net/api/sample.php")
  result = server.call("sample.sumAndDifference", 5, 3)
  pp result

== Documentation

See http://www.ntecs.de/projects/xmlrpc4r.  There is plenty of detail there to
use the client and implement a server.

