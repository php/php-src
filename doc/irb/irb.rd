irb -- interactive ruby
		$Release Version: 0.9 $
		$Revision$
		by Keiju ISHITSUKA(keiju@ishitsuka.com)
		by gotoken-san who is original translater from japanese version

=begin
= What is irb?

irb stands for `interactive ruby'. irb is a tool to execute interactively
ruby expressions read from stdin. 

= Invoking

  % irb

= Usage

Use of irb is easy if you know ruby.  Executing irb, prompts are 
displayed as follows. Then, enter expression of ruby. A input is
executed when it is syntacticaly completed. 

  dim% irb
  irb(main):001:0> 1+2
  3
  irb(main):002:0> class Foo
  irb(main):003:1>  def foo
  irb(main):004:2>    print 1
  irb(main):005:2>  end
  irb(main):006:1> end
  nil
  irb(main):007:0> 

And, Readline extesion module can be used with irb. Using Readline
is the standard default action if Readline is installed. 

= Command line option

  irb.rb [options] file_name opts
  options:
  -f		    suppress read ~/.irbrc 
  -m		    bc mode (fraction or matrix are available)
  -d                set $DEBUG  to true (same as `ruby -d')
  -Kc		    same as `ruby -Kc'
  -r load-module    same as `ruby -r'
  --verbose	    command input is echoed(default)
  --noverbose	    command input isn't echoed
  --echo	    commands are echoed immediately before execution(default)
  --noecho	    commands aren't echoed immediately before execution
  --inspect	    uses `inspect' for output (the default except bc mode)
  --noinspect	    doesn't uses inspect for output
  --readline	    uses Readline extension module
  --noreadline	    doesn't use Readline extension module
  --prompt prompt-mode
  --prompt-mode prompt-mode
		    switches prompt mode. Pre-defined prompt modes are
		    `default', `simple', `xmp' and `inf-ruby'
			    
  --inf-ruby-mode   uses prompt appreciate for inf-ruby-mode on emacs. 
		    Suppresses --readline. 
  --simple-prompt   simple prompt mode
  --noprompt	    no prompt
  --tracer	    display trace for each execution of commands.
  --back-trace-limit n
		    displayes backtrace top n and tail n. The default
		    value is 16. 
  --irb_debug n	    sets internal debug level to n (It shouldn't be used)
  -v, --version	    prints the version of irb

= Configurations

irb reads `~/.irbrc' when it is invoked. If `~/.irbrb' doesn't exist
irb try to read in the order `.irbrc', `irb.rc', `_irbrc' then `$irbrc'. 

The following is altanative to the command line option. To use them
type as follows in an irb session. 

  IRB.conf[:IRB_NAME]="irb"
  IRB.conf[:MATH_MODE]=false
  IRB.conf[:USE_TRACER]=false
  IRB.conf[:USE_LOADER]=false
  IRB.conf[:IGNORE_SIGINT]=true
  IRB.conf[:IGNORE_EOF]=false
  IRB.conf[:INSPECT_MODE]=nil
  IRB.conf[:IRB_RC] = nil
  IRB.conf[:BACK_TRACE_LIMIT]=16
  IRB.conf[:USE_LOADER] = false
  IRB.conf[:USE_READLINE] = nil
  IRB.conf[:USE_TRACER] = false
  IRB.conf[:IGNORE_SIGINT] = true
  IRB.conf[:IGNORE_EOF] = false
  IRB.conf[:PROMPT_MODE] = :DEFALUT
  IRB.conf[:PROMPT] = {...}
  IRB.conf[:DEBUG_LEVEL]=0
  IRB.conf[:VERBOSE]=true

== Customizing prompt

To costomize the prompt you set a variable

  IRB.conf[:PROMPT]

For example, describe as follows in `.irbrc'. 

  IRB.conf[:PROMPT][:MY_PROMPT] = { # name of prompt mode
    :PROMPT_I => nil,		  # normal prompt
    :PROMPT_S => nil,		  # prompt for continuated strings
    :PROMPT_C => nil,		  # prompt for continuated statement
    :RETURN => "    ==>%s\n"	  # format to return value
  }

Then, invoke irb with the above prompt mode by

  % irb --prompt my-prompt

Or add the following in `.irbrc'. 

  IRB.conf[:PROMPT_MODE] = :MY_PROMPT

Constants PROMPT_I, PROMPT_S and PROMPT_C specifies the format. 
In the prompt specification, some special strings are available. 

  %N	command name which is running
  %m	to_s of main object (self)
  %M	inspect of main object (self)
  %l	type of string(", ', /, ]), `]' is inner %w[...]
  %NNi	indent level. NN is degits and means as same as printf("%NNd"). 
        It can be ommited
  %NNn	line number. 
  %%    %

For instance, the default prompt mode is defined as follows:

IRB.conf[:PROMPT_MODE][:DEFAULT] = {
      :PROMPT_I => "%N(%m):%03n:%i> ",
      :PROMPT_S => "%N(%m):%03n:%i%l ",
      :PROMPT_C => "%N(%m):%03n:%i* ",
      :RETURN => "%s\n"
} 

RETURN is used to printf. 

== Configurating subirb

The command line option or IRB.conf specify the default behavior of
(sub)irb. On the other hand, each conf of in the next sction `6. Command' 
is used to individually configurate (sub)irb. 

If proc is set to IRB.conf[:IRB_RC], its subirb will be invoked after
execution of that proc under giving the context of irb as its
aregument. By this mechanism each subirb can be configurated. 

= Command

For irb commands, both simple name and `irb_'-prefixed name are prepared. 

--- exit, quit, irb_exit	
    Quits (sub)irb. 

--- conf, irb_context
    Displays current configuration. Modifing the configuration is
    achieved by sending message to `conf'. 

--- conf.eval_history = N
    Sets execution result history.
    N is a integer or nil. If N > 0, the number of historys is N. 
    If N == 0, the number of historys is unlimited. If N is nill,
    execution result history isn't used(default).

--- conf.back_trace_limit
    Sets display lines of backtrace as top n and tail n. 
    The default value is 16.
    
--- conf.debug_level = N
    Sets debug level of irb. 

--- conf.ignore_eof = true/false
    Whether ^D (control-d) will be ignored or not. 
    If false is set, ^D means quit. 

--- conf.ignore_sigint= true/false
    Whether ^C (control-c) will be ignored or not. 
    If false is set, ^D means quit.  If true, 
      during input:   cancel inputing then return to top level. 
      during execute: abondon current execution. 

--- conf.inf_ruby_mode = true/false
    Whether inf-ruby-mode or not. The default value is false.

--- conf.inspect_mode = true/false/nil
    Specifies inspect mode. 
    true:  display inspect
    false: display to_s
    nil:   inspect mode in non math mode, 
           non inspect mode in math mode. 

--- conf.math_mode
    Whether bc mode or not. 

--- conf.use_loader = true/false
    Whether irb's own file reader method is used when load/require or not. 
    This mode is globaly affected (irb wide). 

--- conf.prompt_c
    prompt for a continuating statement (e.g, immediately after of `if')

--- conf.prompt_i
    standard prompt

--- conf.prompt_s
    prompt for a continuating string

--- conf.rc
    Whether ~/.irbrc is read or not. 

--- conf.use_prompt = true/false
    Prompting or not. 

--- conf.use_readline = true/false/nil
    Whether readline is used or not. 
    true: uses 
    false: doen't use
    nil: intends to use readline except for inf-ruby-mode (default)
#
#--- conf.verbose=T/F
#    Whether verbose messages are display or not. 

--- cws, chws, irb_change_workspace [obj]
    obj will be self. If obj is omitted, self will be home-object, or
    the main object of first started irb.

--- pushws, irb_pushws, irb_push_workspace [obj]
    same as UNIX-shell command pushd.

--- popws, irb_popws, irb_pop_workspace
    same as UNIX-shell command popd

--- irb [obj]
    Invoke subirb. If obj is given, obj will be self. 

--- jobs, irb_jobs
    List of subirb

--- fg n, irb_fg n
    Switch into specified subirb. The following is candidates of n:

      irb number
      thhread
      irb object
      self(obj which is specified of irb obj)

--- kill n, irb_kill n
    Kill subirb. The means of n is as same as the case of irb_fg. 

--- source, irb_source  path
    This is a like UNIX-shell command source. evaluate script in path
    on current context.

--- irb_load path, prev
    irb-version of Ruby's load.

= System variable

--- _  The latest value of evaluation (it is local)
--- __ The history of evaluation values.
    __[line_no] return an evaluation value of line number<line_no>. If
    line_no is a negative, return value before -<line_no> from latest
    value.

= Session Example

  dim% ruby irb.rb
  irb(main):001:0> irb                        # invoke subirb
  irb#1(main):001:0> jobs                     # list of subirbs
  #0->irb on main (#<Thread:0x400fb7e4> : stop)
  #1->irb#1 on main (#<Thread:0x40125d64> : running)
  nil
  irb#1(main):002:0> fg 0                     # switch job
  nil
  irb(main):002:0> class Foo;end
  nil
  irb(main):003:0> irb Foo                    # invoke subirb which has the 
					      #              context of Foo
  irb#2(Foo):001:0> def foo                   # define Foo#foo
  irb#2(Foo):002:1>   print 1
  irb#2(Foo):003:1> end
  nil
  irb#2(Foo):004:0> fg 0                      # switch job
  nil
  irb(main):004:0> jobs                       # list of job
  #0->irb on main (#<Thread:0x400fb7e4> : running)
  #1->irb#1 on main (#<Thread:0x40125d64> : stop)
  #2->irb#2 on Foo (#<Thread:0x4011d54c> : stop)
  nil
  irb(main):005:0> Foo.instance_methods       # Foo#foo is defined asurely
  ["foo"]
  irb(main):006:0> fg 2                       # switch job
  nil
  irb#2(Foo):005:0> def bar                   # define Foo#bar
  irb#2(Foo):006:1>  print "bar"
  irb#2(Foo):007:1> end
  nil
  irb#2(Foo):010:0>  Foo.instance_methods
  ["bar", "foo"]
  irb#2(Foo):011:0> fg 0                      
  nil
  irb(main):007:0> f = Foo.new
  #<Foo:0x4010af3c>
  irb(main):008:0> irb f                      # invoke subirb which has the
					      #  context of f (instance of Foo)
  irb#3(#<Foo:0x4010af3c>):001:0> jobs
  #0->irb on main (#<Thread:0x400fb7e4> : stop)
  #1->irb#1 on main (#<Thread:0x40125d64> : stop)
  #2->irb#2 on Foo (#<Thread:0x4011d54c> : stop)
  #3->irb#3 on #<Foo:0x4010af3c> (#<Thread:0x4010a1e0> : running)
  nil
  irb#3(#<Foo:0x4010af3c>):002:0> foo         # evaluate f.foo
  1nil
  irb#3(#<Foo:0x4010af3c>):003:0> bar         # evaluate f.bar
  barnil
  irb#3(#<Foo:0x4010af3c>):004:0> kill 1, 2, 3# kill job
  nil
  irb(main):009:0> jobs
  #0->irb on main (#<Thread:0x400fb7e4> : running)
  nil
  irb(main):010:0> exit                       # exit
  dim% 

= Restrictions

Because irb evaluates the inputs immediately after the imput is
syntactically completed, irb gives slight different result than
directly use ruby. Known difference is pointed out here. 


== Declaration of the local variable

The following causes an error in ruby:

  eval "foo = 0"
  foo
  --
  -:2: undefined local variable or method `foo' for #<Object:0x40283118> (NameError)
  ---
  NameError

Though, the above will successfully done by irb. 

  >> eval "foo = 0"
 => 0
 >> foo
 => 0

Ruby evaluates a code after reading entire of code and determination
of the scope of local variables. On the other hand, irb do
immediately. More precisely, irb evaluate at first

  evel "foo = 0" 

then foo is defined on this timing. It is because of this
incompatibility.

If you'd like to detect those differences, begin...end can be used:

  >> begin
  ?>   eval "foo = 0"
  >>   foo
  >> end
  NameError: undefined local variable or method `foo' for #<Object:0x4013d0f0>
  (irb):3
  (irb_local_binding):1:in `eval'

== Here-document

Implementation of Here-document is incomplete. 

== Symbol

Irb can not always recognize a symbol as to be Symbol. Concretely, an
expression have completed, however Irb regard it as continuation line.

=end

% Begin Emacs Environment
% Local Variables:
% mode: text
% comment-column: 0
% comment-start: "%"
% comment-end: "\n"
% End:
%
