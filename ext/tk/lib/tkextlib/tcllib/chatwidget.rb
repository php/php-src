#
#  tkextlib/tcllib/chatwidget.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * chatwidget - Provides a multi-paned view suitable for display of
#                  chat room or irc channel information
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('chatwidget', '1.1.0')
TkPackage.require('chatwidget')

module Tk::Tcllib
  class ChatWidget < TkText
    PACKAGE_NAME = 'chatwidget'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('chatwidget')
      rescue
        ''
      end
    end
  end
end

class Tk::Tcllib::ChatWidget
  TkCommandNames = ['::chatwidget::chatwidget'.freeze].freeze

  def show_topic
    tk_send_without_enc('topic', 'show')
    self
  end

  def hide_topic
    tk_send_without_enc('topic', 'hide')
    self
  end

  def set_topic(topic)
    tk_send('topic', 'set', topic)
  end

  def list_name
    tk_split_simplelist(tk_send('name', 'list'))
  end

  def list_name_full
    tk_split_simplelist(tk_send('name', 'list')).map{|lst|
      nick, *opts = tk_split_simplelist(lst)
      h_opt = {}
      opts.slice(2){|k, v| h_opt[k[1..-1]] = tk_tcl2ruby(v)}
      [nick, h_opt]
    }
  end

  def add_name(nick, opts={})
    tk_send('name', 'add', nick, *(hash_kv(opts)))
  end

  def delete_name(nick)
    tk_send('name', 'delete', nick)
  end

  def get_name(nick)
    lst = tk_send('name', 'get', nick)
    return nil if lst.empty?
    nick, *opts = tk_split_simplelist(lst)
    h_opt = {}
    opts.slice(2){|k, v| h_opt[k[1..-1]] = tk_tcl2ruby(v)}
    [nick, h_opt]
  end

  def message(msg, opts={})
    tk_send('message', msg, *(hash_kv(opts)))
    self
  end

  def _parse_hook_list(lst)
    tk_split_simplelist(lst).map{|hook|
      cmd, prior = tk_split_simplelist(hook)
      [procedure(cmd), number(prior)]
    }
  end
  private :_parse_hook_list

  def hook_add(type, *args, &blk) # args -> [prior, cmd], [prior], [cmd]
    #type -> 'message', 'post', 'names_group', 'names_nick', 'chatstate', 'url'

    if prior = args.shift
      if !prior.kind_of?(Numeric)
        cmd = prior
        if (prior = args.shift) && !prior.kind_of?(Numeric)  # error
          args.unshift(prior)
        end
        args.unshift(cmd)
      end
      prior ||= 50  # default priority
    end

    cmd = args.shift || blk

    fail ArgumentError, "invalid arguments" unless args.empty?
    fail ArgumentError, "no callback is given" unless cmd

    _parse_hook_list(tk_send('hook', 'add', type, cmd, prior))
  end

  def hook_remove(type, cmd)
    #type -> 'message', 'post', 'names_group', 'names_nick', 'chatstate', 'url'
    _parse_hook_list(tk_send('hook', 'remove', type, cmd))
  end

  def hook_run(type, *cmd_args)
    #type -> 'message', 'post', 'names_group', 'names_nick', 'chatstate', 'url'
    tk_send('hook', 'run', type, *cmd_args)
  end

  def hook_list(type)
    #type -> 'message', 'post', 'names_group', 'names_nick', 'chatstate', 'url'
    _parse_hook_list(tk_send('hook', 'list', type))
  end

  def show_names
    tk_send('names', 'show')
    self
  end

  def hide_names
    tk_send('names', 'hide')
    self
  end

  def names_widget
    window(tk_send('names'))
  end

  def entry_widget
    window(tk_send('entry'))
  end

  def chat_widget
    window(tk_send('chat'))
  end
end
