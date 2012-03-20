#
# tk/image.rb : treat Tk image objects
#

require 'tk'

class TkImage<TkObject
  include Tk

  TkCommandNames = ['image'.freeze].freeze

  Tk_IMGTBL = TkCore::INTERP.create_table

  (Tk_Image_ID = ['i'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    Tk_IMGTBL.mutex.synchronize{ Tk_IMGTBL.clear }
  }

  def self.new(keys=nil)
    if keys.kind_of?(Hash)
      name = nil
      if keys.key?(:imagename)
        name = keys[:imagename]
      elsif keys.key?('imagename')
        name = keys['imagename']
      end
      if name
        if name.kind_of?(TkImage)
          obj = name
        else
          name = _get_eval_string(name)
          obj = nil
          Tk_IMGTBL.mutex.synchronize{
            obj = Tk_IMGTBL[name]
          }
        end
        if obj
          if !(keys[:without_creating] || keys['without_creating'])
            keys = _symbolkey2str(keys)
            keys.delete('imagename')
            keys.delete('without_creating')
            obj.instance_eval{
              tk_call_without_enc('image', 'create',
                                  @type, @path, *hash_kv(keys, true))
            }
          end
          return obj
        end
      end
    end
    (obj = self.allocate).instance_eval{
      Tk_IMGTBL.mutex.synchronize{
        initialize(keys)
        Tk_IMGTBL[@path] = self
      }
    }
    obj
  end

  def initialize(keys=nil)
    @path = nil
    without_creating = false
    if keys.kind_of?(Hash)
      keys = _symbolkey2str(keys)
      @path = keys.delete('imagename')
      without_creating = keys.delete('without_creating')
    end
    unless @path
      Tk_Image_ID.mutex.synchronize{
        @path = Tk_Image_ID.join(TkCore::INTERP._ip_id_)
        Tk_Image_ID[1].succ!
      }
    end
    unless without_creating
      tk_call_without_enc('image', 'create',
                          @type, @path, *hash_kv(keys, true))
    end
  end

  def delete
    Tk_IMGTBL.mutex.synchronize{
      Tk_IMGTBL.delete(@id) if @id
    }
    tk_call_without_enc('image', 'delete', @path)
    self
  end
  def height
    number(tk_call_without_enc('image', 'height', @path))
  end
  def inuse
    bool(tk_call_without_enc('image', 'inuse', @path))
  end
  def itemtype
    tk_call_without_enc('image', 'type', @path)
  end
  def width
    number(tk_call_without_enc('image', 'width', @path))
  end

  def TkImage.names
    Tk_IMGTBL.mutex.synchronize{
      Tk.tk_call_without_enc('image', 'names').split.collect!{|id|
        (Tk_IMGTBL[id])? Tk_IMGTBL[id] : id
      }
    }
  end

  def TkImage.types
    Tk.tk_call_without_enc('image', 'types').split
  end
end

class TkBitmapImage<TkImage
  def __strval_optkeys
    super() + ['maskdata', 'maskfile']
  end
  private :__strval_optkeys

  def initialize(*args)
    @type = 'bitmap'
    super(*args)
  end
end

# A photo is an image whose pixels can display any color or be transparent.
# At present, only GIF and PPM/PGM formats are supported, but an interface
# exists to allow additional image file formats to be added easily.
# 
# This class documentation is a copy from the original Tcl/Tk at
# http://www.tcl.tk/man/tcl8.5/TkCmd/photo.htm with some rewrited parts.
class TkPhotoImage<TkImage
  NullArgOptionKeys = [ "shrink", "grayscale" ]

  def _photo_hash_kv(keys)
    keys = _symbolkey2str(keys)
    NullArgOptionKeys.collect{|opt|
      if keys[opt]
        keys[opt] = None
      else
        keys.delete(opt)
      end
    }
    keys.collect{|k,v|
      ['-' << k, v]
    }.flatten
  end
  private :_photo_hash_kv

  # Create a new image with the given options.
  # == Examples of use :
  # === Create an empty image of 300x200 pixels
  #
  #		image = TkPhotoImage.new(:height => 200, :width => 300)
  #
  # === Create an image from a file
  #
  #		image = TkPhotoImage.new(:file: => 'my_image.gif')
  #
  # == Options
  # Photos support the following options: 
  # * :data
  #   Specifies the contents of the image as a string.
  # * :format
  #   Specifies the name of the file format for the data.
  # * :file
  #   Gives the name of a file that is to be read to supply data for the image. 
  # * :gamma
  #   Specifies that the colors allocated for displaying this image in a window
  #   should be corrected for a non-linear display with the specified gamma
  #   exponent value.
  # * height
  #   Specifies the height of the image, in pixels. This option is useful
  #   primarily in situations where the user wishes to build up the contents of
  #   the image piece by piece. A value of zero (the default) allows the image
  #   to expand or shrink vertically to fit the data stored in it.
  # * palette
  #   Specifies the resolution of the color cube to be allocated for displaying
  #   this image.
  # * width
  #   Specifies the width of the image, in pixels. This option is useful
  #   primarily in situations where the user wishes to build up the contents of
  #   the image piece by piece. A value of zero (the default) allows the image
  #   to expand or shrink horizontally to fit the data stored in it. 
  def initialize(*args)
    @type = 'photo'
    super(*args)
  end

  # Blank the image; that is, set the entire image to have no data, so it will
  # be displayed as transparent, and the background of whatever window it is
  # displayed in will show through. 
  def blank
    tk_send_without_enc('blank')
    self
  end

  def cget_strict(option)
    case option.to_s
    when 'data', 'file'
      tk_send 'cget', '-' << option.to_s
    else
      tk_tcl2ruby(tk_send('cget', '-' << option.to_s))
    end
  end

  # Returns the current value of the configuration option given by option.
  # Example, display name of the file from which <tt>image</tt> was created:
  # 	puts image.cget :file
  def cget(option)
    unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      cget_strict(option)
    else
      begin
        cget_strict(option)
      rescue => e
        if current_configinfo.has_key?(option.to_s)
          # error on known option
          fail e
        else
          # unknown option
          nil
        end
      end
    end
  end

  # Copies a region from the image called source to the image called
  # destination, possibly with pixel zooming and/or subsampling. If no options
  # are specified, this method copies the whole of source into destination,
  # starting at coordinates (0,0) in destination. The following options may be
  # specified: 
  #
  # * :from [x1, y1, x2, y2]
  #   Specifies a rectangular sub-region of the source image to be copied.
  #   (x1,y1) and (x2,y2) specify diagonally opposite corners of the rectangle.
  #   If x2 and y2 are not specified, the default value is the bottom-right
  #   corner of the source image. The pixels copied will include the left and
  #   top edges of the specified rectangle but not the bottom or right edges.
  #   If the :from option is not given, the default is the whole source image.
  # * :to [x1, y1, x2, y2]
  #   Specifies a rectangular sub-region of the destination image to be
  #   affected. (x1,y1) and (x2,y2) specify diagonally opposite corners of the
  #   rectangle. If x2 and y2 are not specified, the default value is (x1,y1)
  #   plus the size of the source region (after subsampling and zooming, if
  #   specified). If x2 and  y2 are specified, the source region will be
  #   replicated if necessary to fill the destination region in a tiled fashion.
  # * :shrink
  #   Specifies that the size of the destination image should be reduced, if 
  #   necessary, so that the region being copied into is at the bottom-right 
  #   corner of the image. This option will not affect the width or height of 
  #   the image if the user has specified a non-zero value for the :width or
  #   :height configuration option, respectively.
  # * :zoom [x, y]
  #   Specifies that the source region should be magnified by a factor of x 
  #   in the X direction and y in the Y direction. If y is not given, the
  #   default value is the same as x. With this option, each pixel in the
  #   source image will be expanded into a block of x x y pixels in the
  #   destination image, all the same color. x and y must be greater than 0.
  # * :subsample [x, y]
  #   Specifies that the source image should be reduced in size by using only
  #   every xth pixel in the X direction and yth pixel in the Y direction. 
  #   Negative values will cause the image to be flipped about the Y or X axes, 
  #   respectively. If y is not given, the default value is the same as x.
  # * :compositingrule rule
  #   Specifies how transparent pixels in the source image are combined with 
  #   the destination image. When a compositing rule of <tt>overlay</tt> is set,
  #   the old  contents of the destination image are visible, as if the source
  #   image were  printed on a piece of transparent film and placed over the
  #   top of the  destination. When a compositing rule of <tt>set</tt> is set,
  #   the old contents of  the destination image are discarded and the source
  #   image is used as-is. The default compositing rule is <tt>overlay</tt>. 
  def copy(src, *opts)
    if opts.size == 0
      tk_send('copy', src)
    elsif opts.size == 1 && opts[0].kind_of?(Hash)
      tk_send('copy', src, *_photo_hash_kv(opts[0]))
    else
      # for backward compatibility
      args = opts.collect{|term|
        if term.kind_of?(String) && term.include?(?\s)
          term.split
        else
          term
        end
      }.flatten
      tk_send('copy', src, *args)
    end
    self
  end

  # Returns image data in the form of a string. The following options may be
  # specified:
  # * :background color
  #   If the color is specified, the data will not contain any transparency
  #   information. In all transparent pixels the color will be replaced by the
  #   specified color.
  # * :format format-name
  #   Specifies the name of the image file format handler to be used.
  #   Specifically, this subcommand searches for the first handler whose name
  #   matches an initial substring of format-name and which has the capability
  #   to read this image data. If this option is not given, this subcommand
  #   uses the first handler that has the capability to read the image data.
  # * :from [x1, y1, x2, y2]
  #   Specifies a rectangular region of imageName to be returned. If only x1
  #   and y1 are specified, the region extends from (x1,y1) to the bottom-right
  #   corner of imageName. If all four coordinates are given, they specify
  #   diagonally opposite corners of the rectangular region, including x1,y1
  #   and excluding x2,y2. The default, if this option is not given, is the
  #   whole image.
  # * :grayscale
  #   If this options is specified, the data will not contain color information.
  #   All pixel data will be transformed into grayscale. 
  def data(keys={})
    tk_split_list(tk_send('data', *_photo_hash_kv(keys)))
  end

  # Returns the color of the pixel at coordinates (x,y) in the image as a list 
  # of three integers between 0 and 255, representing the red, green and blue
  # components respectively. 
  def get(x, y)
    tk_send('get', x, y).split.collect{|n| n.to_i}
  end

  def put(data, *opts)
    if opts.empty?
      tk_send('put', data)
    elsif opts.size == 1 && opts[0].kind_of?(Hash)
      tk_send('put', data, *_photo_hash_kv(opts[0]))
    else
      # for backward compatibility
      tk_send('put', data, '-to', *opts)
    end
    self
  end

  def read(file, *opts)
    if opts.size == 0
      tk_send('read', file)
    elsif opts.size == 1 && opts[0].kind_of?(Hash)
      tk_send('read', file, *_photo_hash_kv(opts[0]))
    else
      # for backward compatibility
      args = opts.collect{|term|
        if term.kind_of?(String) && term.include?(?\s)
          term.split
        else
          term
        end
      }.flatten
      tk_send('read', file, *args)
    end
    self
  end

  def redither
    tk_send 'redither'
    self
  end

  # Returns a boolean indicating if the pixel at (x,y) is transparent. 
  def get_transparency(x, y)
    bool(tk_send('transparency', 'get', x, y))
  end
	
  # Makes the pixel at (x,y) transparent if <tt>state</tt> is true, and makes
  # that pixel opaque otherwise. 
  def set_transparency(x, y, state)
    tk_send('transparency', 'set', x, y, state)
    self
  end

  def write(file, *opts)
    if opts.size == 0
      tk_send('write', file)
    elsif opts.size == 1 && opts[0].kind_of?(Hash)
      tk_send('write', file, *_photo_hash_kv(opts[0]))
    else
      # for backward compatibility
      args = opts.collect{|term|
        if term.kind_of?(String) && term.include?(?\s)
          term.split
        else
          term
        end
      }.flatten
      tk_send('write', file, *args)
    end
    self
  end
end
