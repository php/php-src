#
#   push-ws.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

module IRB
  class Context

    def irb_level
      workspace_stack.size
    end

    def workspaces
      if defined? @workspaces
	@workspaces
      else
	@workspaces = []
      end
    end

    def push_workspace(*_main)
      if _main.empty?
	if workspaces.empty?
	  print "No other workspace\n"
	  return nil
	end
	ws = workspaces.pop
	workspaces.push @workspace
	@workspace = ws
	return workspaces
      end

      workspaces.push @workspace
      @workspace = WorkSpace.new(@workspace.binding, _main[0])
      if !(class<<main;ancestors;end).include?(ExtendCommandBundle)
	main.extend ExtendCommandBundle
      end
    end

    def pop_workspace
      if workspaces.empty?
	print "workspace stack empty\n"
	return
      end
      @workspace = workspaces.pop
    end
  end
end

