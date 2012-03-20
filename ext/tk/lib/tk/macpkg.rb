#
#   tk/macpkg.rb : methods for Tcl/Tk packages for Macintosh
#                     2000/11/22 by Hidetoshi Nagai <nagai@ai.kyutech.ac.jp>
#
#     ATTENTION !!
#         This is NOT TESTED. Because I have no test-environment.
#
#
require 'tk'

module Tk
  def Tk.load_tclscript_rsrc(resource_name, file=None)
    # Mac only
    tk_call('source', '-rsrc', resource_name, file)
  end

  def Tk.load_tclscript_rsrcid(resource_id, file=None)
    # Mac only
    tk_call('source', '-rsrcid', resource_id, file)
  end
end

module Tk::MacResource
end
#TkMacResource = Tk::MacResource
#Tk.__set_toplevel_aliases__(:Tk, Tk::MacResource, :TkMacResource)
Tk.__set_loaded_toplevel_aliases__('tk/macpkg.rb', :Tk, Tk::MacResource,
                                   :TkMacResource)

module Tk::MacResource
  extend Tk
  extend Tk::MacResource

  TkCommandNames = ['resource'.freeze].freeze

  PACKAGE_NAME = 'resource'.freeze
  def self.package_name
    PACKAGE_NAME
  end

  tk_call_without_enc('package', 'require', 'resource')

  def close(rsrcRef)
    tk_call('resource', 'close', rsrcRef)
  end

  def delete(rsrcType, opts=nil)
    tk_call('resource', 'delete', *(hash_kv(opts) << rsrcType))
  end

  def files(rsrcRef=nil)
    if rsrcRef
      tk_call('resource', 'files', rsrcRef)
    else
      tk_split_simplelist(tk_call('resource', 'files'))
    end
  end

  def list(rsrcType, rsrcRef=nil)
    tk_split_simplelist(tk_call('resource', 'list', rsrcType, rsrcRef))
  end

  def open(fname, access=nil)
    tk_call('resource', 'open', fname, access)
  end

  def read(rsrcType, rsrcID, rsrcRef=nil)
    tk_call('resource', 'read', rsrcType, rsrcID, rsrcRef)
  end

  def types(rsrcRef=nil)
    tk_split_simplelist(tk_call('resource', 'types', rsrcRef))
  end

  def write(rsrcType, data, opts=nil)
    tk_call('resource', 'write', *(hash_kv(opts) << rsrcType << data))
  end

  module_function :close, :delete, :files, :list, :open, :read, :types, :write
end
