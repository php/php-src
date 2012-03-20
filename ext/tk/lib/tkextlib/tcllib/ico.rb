#
#  tkextlib/tcllib/ico.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Reading and writing windows icons
#

require 'tk'
require 'tk/image'
#require 'tkextlib/tcllib.rb'

# TkPackage.require('ico', '0.3')
TkPackage.require('ico')

module Tk
  module Tcllib
    class ICO < TkImage
      PACKAGE_NAME = 'ico'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('ico')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::ICO
  def self.list(file, keys=nil)
    tk_split_list(tk_call_without_enc('::ico::getIconList', file,
                                      *hash_kv(keys, true)))
  end

  def self.icons(file, keys=nil)
    tk_split_simplelist(tk_call_without_enc('::ico::icons', file,
                                            *hash_kv(keys, true))).map{|elem|
      num_or_str(elem)
    }
  end

  def self.get_members(file, name, keys=nil)
    tk_split_simplelist(tk_call_without_enc('::ico::getMembers', file, name,
                                            *hash_kv(keys, true))).map{|elem|
      name, width, height, bpp =  tk_split_simplelist(elem)
      [name, number(width), number(height), number(bpp)]
    }
  end

  def self.get(file, index, keys=nil)
    tk_call_without_enc('::ico::getIcon', file, index, *hash_kv(keys, true))
  end
  def self.get_icon(*args)
    get(*args)
  end

  def self.get_by_name(file, name, keys=nil)
    tk_call_without_enc('::ico::getIconByName', file, name,
                        *hash_kv(keys, true))
  end
  def self.get_icon_by_name(*args)
    get_by_name(*args)
  end

  def self.get_fileicon(file, keys=nil)
    tk_call_without_enc('::ico::getFileIcon', file, *hash_kv(keys, true))
  end

  def self.get_image(file, index, keys={})
    keys = _symbolkey2str(keys)
    keys.delete('format')
    self.new(file, index, keys)
  end

  def self.get_data(file, index, keys={})
    keys['format'] = 'data'
    tk_split_list(tk_call_without_enc('::ico::getIcon', file, index,
                                      *hash_kv(keys, true)))
  end

  def self.write(file, index, depth, data, keys=nil)
    tk_call_without_enc('::ico::writeIcon', file, index, depth, data,
                        *hash_kv(keys, true))
  end

  def self.copy(from_file, from_index, to_file, to_index, keys=nil)
    tk_call_without_enc('::ico::copyIcon',
                        from_file, from_index, to_file, to_index,
                        *hash_kv(keys, true))
  end

  def self.exe_to_ico(exe_file, ico_file, keys=nil)
    tk_call_without_enc('::ico::copyIcon', exe_file, ico_file,
                        *hash_kv(keys, true))
  end

  def self.clear_cache(file=None)
    tk_call_without_enc('::ico::clearCache', file)
  end

  def self.transparent_color(image, color)
    if image.kind_of?(Array)
      tk_split_list(tk_call_without_enc('::ico::transparentColor',
                                        image, color))
    else
      tk_call_without_enc('::ico::transparentColor', image, color)
    end
  end

  def self.show(file, keys=nil)
    tk_call_without_enc('::ico::Show', file, *hash_kv(keys, true))
  end

  ###########################

  def initialize(file, index, keys=nil)
    keys = _symbolkey2str(keys)
    if keys.key?('name')
      @path = keys['name'].to_s
    else
      Tk_Image_ID.mutex.synchronize{
        @path = Tk_Image_ID.join(TkCore::INTERP._ip_id_)
        Tk_Image_ID[1].succ!
      }
    end
    tk_call_without_enc('::ico::getIcon', file, index, '-name', @path,
                        '-format', 'image', *hash_kv(keys, true))
    Tk_IMGTBL[@path] = self
  end

  def write(file, index, depth, keys=nil)
    Tk::Tcllib::ICO.write(file, index, depth, @path, keys=nil)
    self
  end

  def transparent_color(color)
    tk_call_without_enc('::ico::transparentColor', @path, color)
    self
  end
end
