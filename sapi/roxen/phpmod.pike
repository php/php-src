/* Roxen PHP module based of the Roxen CGI module for Roxen 1.4. */

#include <roxen.h>
#include <module.h>
inherit "module";
inherit "roxenlib";

constant cvs_version = "$Id$";
constant thread_safe = 1;

string trim( string what )
{
  sscanf(what, "%*[ \t]%s", what);
  what = reverse(what);
  sscanf(what, "%*[ \t]%s", what);
  what = reverse(what);
  return what;
}

//#define PHP_DEBUG
#ifdef PHP_DEBUG
#define DWERROR(X)	report_debug("Thr("+getpid()+"): "+X)
#else /* !PHP_DEBUG */
#define DWERROR(X)
#endif /* PHP_DEBUG */

array register_module()
{
  return 
  ({
    MODULE_FILE_EXTENSION | MODULE_PARSER,
    "PHP Script Support", 
    "This module allows Roxen users to run PHP scripts, optionally in "
    "combination with RXML. ",
  });
}

class PHPScript
{
  object interpretor;
  string command;
  string buffer="";
  // stderr is handled by run().
  mapping (string:string) environment;
  int blocking, written, close_when_done;
  object mid;
  void done() {
    if(strlen(buffer)) {
      close_when_done = 1;
      if(QUERY(rxml)) {
	buffer = parse_rxml(buffer, mid);
	write_callback();
      }
    } else
      destruct();
  }

  void destroy() {
    mid->do_not_disconnect = 0;
    //    destruct(interpretor);
    mid->file = ([ "len": written, "raw":1 ]);
    //    mid->do_log();
  }
  void write_callback() 
  {
    DWERROR("PHP:Wrapper::write_callback()\n");
    if(!strlen(buffer)) 
      return;
    //    int nelems = tofd->write( buffer );
    int nelems;
    array err = catch { nelems = mid->my_fd->write(buffer); };
    DWERROR(sprintf("PHP:Wrapper::write_callback(): write(%O) => %d\n",
		    buffer, nelems));
    if( err || nelems < 0 )
      // if nelems == 0, network buffer is full. We still want to continue.
    {
      buffer="";
      close_when_done = -1;
    } else {
      written += nelems;
      buffer = buffer[nelems..]; 
      DWERROR(sprintf("Done: %d %d...\n", strlen(buffer), close_when_done));
      if(close_when_done && !strlen(buffer)) {
        destruct();
      }
    }
  }

  int write( string what )
  {
    DWERROR(sprintf("PHP:Wrapper::write(%O)\n", what));
    if(close_when_done == -1) // Remote closed
      return -1;
    if(buffer == "" )
    {
      buffer = what;
      if(!QUERY(rxml)) write_callback();
    } else
      buffer += what;
    return strlen(what);
  }

  void send_headers(int code, mapping headers) 
  {
    DWERROR(sprintf("PHP:PHPWrapper::send_headers(%d,%O)\n", code, headers));
    string result = "", post="";
    string code = mid->errors[code||200];
    int ct_received = 0, sv_received = 0;
    if(headers)
      foreach(indices(headers), string header)
      {
	string value = headers[header];
	if(!header || !value)
	{
	  // Heavy DWIM. For persons who forget about headers altogether.
	  continue;
	}
	header = trim(header);
	value = trim(value);
	switch(lower_case( header ))
	{
	case "status":
	  code = value;
	  break;

	case "content-type":
	  ct_received=1;
	  result += header+": "+value+"\r\n";
	  break;

	case "server":
	  sv_received=1;
	  result += header+": "+value+"\r\n";
	  break;

	case "location":
	  code = "302 Redirection";
	  result += header+": "+value+"\r\n";
	  break;

	default:
	  result += header+": "+value+"\r\n";
	  break;
	}
      }
    if(!sv_received)
      result += "Server: "+roxen.version()+"/PHP\r\n";
    if(!ct_received)
      result += "Content-Type: text/html\r\n";
    write("HTTP/1.0 "+code+"\r\n"+result+"\r\n");
  }

  PHPScript run()
  {
    DWERROR("PHP:PHPScript::run()\n");
    //    if( QUERY(rxml) )
      //      stdout = (wrapper = RXMLWrapper( stdout, mid ))->get_fd();
    mapping options = ([
      "env":environment,
    ]);
#if 1
    if(!QUERY(rxml)) {
      mid->my_fd->set_blocking();
      options->my_fd = mid->my_fd;
    }
#endif
    mid->my_fd->set_close_callback(done);
    interpretor->run(command, options, this_object(), done);
    return this_object();
  }


  void create( object id )
  {
    DWERROR("PHP:PHPScript()\n");
    interpretor = PHP4.Interpretor();
    mid = id;

#ifndef THREADS
    if(id->misc->orig) // An <insert file=...> operation, and we have no threads.
      blocking = 1;
#else
    if(id->misc->orig && this_thread() == roxen.backend_thread)
      blocking = 1; 
    // An <insert file=...> and we are 
    // currently in the backend thread.
#endif
    if(!id->realfile)
    {
      id->realfile = id->conf->real_file( id->not_query, id );
      if(!id->realfile)
        error("No real file associated with "+id->not_query+
              ", thus it's not possible to run it as a PHP script.\n");
    }
    command = id->realfile;

    environment =(QUERY(env)?getenv():([]));
    environment |= global_env;
    environment |= build_env_vars( id->realfile, id, id->misc->path_info );
    environment |= build_roxen_env_vars(id);
    if(id->misc->ssi_env)     	environment |= id->misc->ssi_env;
    if(id->misc->is_redirected) environment["REDIRECT_STATUS"] = "1";
    if(id->rawauth && QUERY(rawauth))
      environment["HTTP_AUTHORIZATION"] = (string)id->rawauth;
    else
      m_delete(environment, "HTTP_AUTHORIZATION");
    if(QUERY(clearpass) && id->auth && id->realauth ) {
      environment["REMOTE_USER"] = (id->realauth/":")[0];
      environment["REMOTE_PASSWORD"] = (id->realauth/":")[1];
    } else {
      m_delete(environment, "REMOTE_PASSWORD");
    }
    if (id->rawauth) {
      environment["AUTH_TYPE"] = (id->rawauth/" ")[0];
    }
    //    DWERROR(sprintf("%O\n", environment));
    //    ffd = id->my_fd;
  }
}

mapping(string:string) global_env = ([]);
void start(int n, object conf)
{
  DWERROR("PHP:start()\n");

  module_dependencies(conf, ({ "pathinfo" }));
  if(conf)
  {
    string tmp=conf->query("MyWorldLocation");
    sscanf(tmp, "%*s//%s", tmp);
    sscanf(tmp, "%s:", tmp);
    sscanf(tmp, "%s/", tmp);
    global_env["SERVER_NAME"]=tmp;
    global_env["SERVER_SOFTWARE"]=roxen.version();
    global_env["GATEWAY_INTERFACE"]="PHP/1.1";
    global_env["SERVER_PROTOCOL"]="HTTP/1.0";
    global_env["SERVER_URL"]=conf->query("MyWorldLocation");

    array us = ({0,0});
    foreach(query("extra_env")/"\n", tmp)
      if(sscanf(tmp, "%s=%s", us[0], us[1])==2)
        global_env[us[0]] = us[1];
  }
}
mapping handle_file_extension(object o, string e, object id)
{
  DWERROR("PHP:handle_file_extension()\n");
  id->do_not_disconnect = 1;
  PHPScript( id )->run();
  return http_pipe_in_progress();
}
/*
** Variables et. al.
*/
array (string) query_file_extensions()
{
  return QUERY(ext);
}


void create(object conf)
{
  defvar("env", 0, "Pass environment variables", TYPE_FLAG,
	 "If this is set, all environment variables roxen has will be "
         "passed to PHP scripts, not only those defined in the PHP/1.1 standard. "
         "This includes PATH. (For a quick test, try this script with "
	 "and without this variable set:"
	 "<pre>"
	 "#!/bin/sh\n\n"
         "echo Content-type: text/plain\n"
	 "echo ''\n"
	 "env\n"
	 "</pre>)");

  defvar("rxml", 0, "Parse RXML in PHP-scripts", TYPE_FLAG,
	 "If this is set, the output from PHP-scripts handled by this "
         "module will be RXMl parsed. NOTE: No data will be returned to the "
         "client until the PHP-script is fully parsed.");

  defvar("extra_env", "", "Extra environment variables", TYPE_TEXT_FIELD,
	 "Extra variables to be sent to the script, format:<pre>"
	 "NAME=value\n"
	 "NAME=value\n"
	 "</pre>Please note that the standard variables will have higher "
	 "priority.");

  defvar("ext",
	 ({"php", "php3", "php4"
	 }), "PHP-script extensions", TYPE_STRING_LIST,
         "All files ending with these extensions, will be parsed as "+
	 "PHP-scripts.");

  defvar("rawauth", 0, "Raw user info", TYPE_FLAG|VAR_MORE,
	 "If set, the raw, unparsed, user info will be sent to the script, "
	 " in the HTTP_AUTHORIZATION environment variable. This is not "
	 "recommended, but some scripts need it. Please note that this "
	 "will give the scripts access to the password used.");

  defvar("clearpass", 0, "Send decoded password", TYPE_FLAG|VAR_MORE,
	 "If set, the variable REMOTE_PASSWORD will be set to the decoded "
	 "password value.");

  defvar( "cgi_tag", 1, "Provide the &lt;cgi&gt; tag", TYPE_FLAG,
           "If set, the &lt;cgi&gt; tag will be available" );
}

int|string tag_cgi( string tag, mapping args, object id )
{
  DWERROR("PHP:tag_cgi()\n");

  if(!query("cgi_tag")) 
    return 0;

  if(args->help)
    return ("<b>&lt;"+tag+" script=path [cache=seconds] [default-argument=value] "
            "[argument=value]&gt;:</b>");

  if(!args->cache) 
    NOCACHE();
  else
    CACHE( (int)args->cache || 60 );


  object fid = id->clone_me();
  string file = args->script;
  if(!file)
    return "No 'script' argument to the PHP tag";
  fid->not_query = fix_relative( file, id );
  foreach(indices(args), string arg )
  {
    if(arg == "script")
      continue;
    if(arg == "cache")
      continue;
    if(arg[..7] == "default-")
    {
      if(!id->variables[arg[8..]])
        fid->variables[arg[8..]] = args[arg];
    }
    else
      fid->variables[arg] = args[arg];
  }
  fid->realfile=0;
  fid->method = "GET";
  mixed e = catch 
  {
    string data=handle_file_extension( 0, "cgi", fid )->file->read();
    if(!sscanf(data, "%*s\r\n\r\n%s", data))
      sscanf(data, "%*s\n\n%s", data);
    return data;
  };
  return ("Failed to run PHP script: <font color=red><pre>"+
          (html_encode_string(describe_backtrace(e))/"\n")[0]+
          "</pre></font>");
}


mapping query_tag_callers()
{
  return ([
    "cgi":tag_cgi,
  ]);
}
