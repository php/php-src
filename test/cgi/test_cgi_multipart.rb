require 'test/unit'
require 'cgi'
require 'tempfile'
require 'stringio'


##
## usage:
##   boundary = 'foobar1234'  # or nil
##   multipart = MultiPart.new(boundary)
##   multipart.append('name1', 'value1')
##   multipart.append('file1', File.read('file1.html'), 'file1.html')
##   str = multipart.close()
##   str.each_line {|line| p line }
##   ## output:
##   # "--foobar1234\r\n"
##   # "Content-Disposition: form-data: name=\"name1\"\r\n"
##   # "\r\n"
##   # "value1\r\n"
##   # "--foobar1234\r\n"
##   # "Content-Disposition: form-data: name=\"file1\"; filename=\"file1.html\"\r\n"
##   # "Content-Type: text/html\r\n"
##   # "\r\n"
##   # "<html>\n"
##   # "<body><p>Hello</p></body>\n"
##   # "</html>\n"
##   # "\r\n"
##   # "--foobar1234--\r\n"
##
class MultiPart

  def initialize(boundary=nil)
    @boundary = boundary || create_boundary()
    @buf = ''
    @buf.force_encoding(::Encoding::ASCII_8BIT) if defined?(::Encoding)
  end
  attr_reader :boundary

  def append(name, value, filename=nil, content_type=nil)
    content_type = detect_content_type(filename) if filename && content_type.nil?
    s = filename ? "; filename=\"#{filename}\"" : ''
    buf = @buf
    buf << "--#{boundary}\r\n"
    buf << "Content-Disposition: form-data: name=\"#{name}\"#{s}\r\n"
    buf << "Content-Type: #{content_type}\r\n" if content_type
    buf << "\r\n"
    value = value.dup.force_encoding(::Encoding::ASCII_8BIT) if defined?(::Encoding)
    buf << value
    buf << "\r\n"
    return self
  end

  def close
    buf = @buf
    @buf = ''
    return buf << "--#{boundary}--\r\n"
  end

  def create_boundary()  #:nodoc:
    return "--boundary#{rand().to_s[2..-1]}"
  end

  def detect_content_type(filename)   #:nodoc:
    filename =~ /\.(\w+)\z/
    return MIME_TYPES[$1] || 'application/octet-stream'
  end

  MIME_TYPES = {
    'gif'      =>  'image/gif',
    'jpg'      =>  'image/jpeg',
    'jpeg'     =>  'image/jpeg',
    'png'      =>  'image/png',
    'bmp'      =>  'image/bmp',
    'tif'      =>  'image/tiff',
    'tiff'     =>  'image/tiff',
    'htm'      =>  'text/html',
    'html'     =>  'text/html',
    'xml'      =>  'text/xml',
    'txt'      =>  'text/plain',
    'text'     =>  'text/plain',
    'css'      =>  'text/css',
    'mpg'      =>  'video/mpeg',
    'mpeg'     =>  'video/mpeg',
    'mov'      =>  'video/quicktime',
    'avi'      =>  'video/x-msvideo',
    'mp3'      =>  'audio/mpeg',
    'mid'      =>  'audio/midi',
    'wav'      =>  'audio/x-wav',
    'zip'      =>  'application/zip',
    #'tar.gz'   =>  'application/gtar',
    'gz'       =>  'application/gzip',
    'bz2'      =>  'application/bzip2',
    'rtf'      =>  'application/rtf',
    'pdf'      =>  'application/pdf',
    'ps'       =>  'application/postscript',
    'js'       =>  'application/x-javascript',
    'xls'      =>  'application/vnd.ms-excel',
    'doc'      =>  'application/msword',
    'ppt'      =>  'application/vnd.ms-powerpoint',
  }

end



class CGIMultipartTest < Test::Unit::TestCase

  def setup
    ENV['REQUEST_METHOD'] = 'POST'
  end

  def teardown
    %w[ REQUEST_METHOD CONTENT_TYPE CONTENT_LENGTH REQUEST_METHOD ].each do |name|
      ENV.delete(name)
    end
    $stdin.close() if $stdin.is_a?(Tempfile)
    $stdin = STDIN
  end

  def _prepare(data)
    ## create multipart input
    multipart = MultiPart.new(@boundary)
    data.each do |hash|
      multipart.append(hash[:name], hash[:value], hash[:filename])
    end
    input = multipart.close()
    input = yield(input) if block_given?
    #$stderr.puts "*** debug: input=\n#{input.collect{|line| line.inspect}.join("\n")}"
    @boundary ||= multipart.boundary
    ## set environment
    ENV['CONTENT_TYPE'] = "multipart/form-data; boundary=#{@boundary}"
    ENV['CONTENT_LENGTH'] = input.length.to_s
    ENV['REQUEST_METHOD'] = 'POST'
    ## set $stdin
    tmpfile = Tempfile.new('test_cgi_multipart')
    tmpfile.binmode
    tmpfile << input
    tmpfile.rewind()
    $stdin = tmpfile
  end

  def _test_multipart
    caller(0).find {|s| s =~ /in `test_(.*?)'/ }
    testname = $1
    #$stderr.puts "*** debug: testname=#{testname.inspect}"
    _prepare(@data)
    cgi = RUBY_VERSION>="1.9" ? CGI.new(:accept_charset=>"UTF-8") : CGI.new
    expected_names = @data.collect{|hash| hash[:name] }.sort
    assert_equal(expected_names, cgi.params.keys.sort)
    threshold = 1024*10
    @data.each do |hash|
      name = hash[:name]
      expected = hash[:value]
      if RUBY_VERSION>="1.9"
        if hash[:filename] #if file
          expected_class = @expected_class || (hash[:value].length < threshold ? StringIO : Tempfile)
          assert(cgi.files.keys.member?(hash[:name]))
        else
          expected_class = String
          assert_equal(expected, cgi[name])
          assert_equal(false,cgi.files.keys.member?(hash[:name]))
        end
      else
        expected_class = @expected_class || (hash[:value].length < threshold ? StringIO : Tempfile)
      end
      assert_kind_of(expected_class, cgi[name])
      assert_equal(expected, cgi[name].read())
      assert_equal(hash[:filename] || '', cgi[name].original_filename)  #if hash[:filename]
      assert_equal(hash[:content_type] || '', cgi[name].content_type)  #if hash[:content_type]
    end
  end


  def _read(basename)
    filename = File.join(File.dirname(__FILE__), 'testdata', basename)
    s = File.open(filename, 'rb') {|f| f.read() }

    return s
  end


  def test_cgi_multipart_stringio
    @boundary = '----WebKitFormBoundaryAAfvAII+YL9102cX'
    @data = [
      {:name=>'hidden1', :value=>'foobar'},
      {:name=>'text1',   :value=>"\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\xE3\x81\x88\xE3\x81\x8A"},
      {:name=>'file1',   :value=>_read('file1.html'),
       :filename=>'file1.html', :content_type=>'text/html'},
      {:name=>'image1',  :value=>_read('small.png'),
       :filename=>'small.png',  :content_type=>'image/png'},  # small image
    ]
    @data[1][:value].force_encoding(::Encoding::UTF_8) if defined?(::Encoding)
    @expected_class = StringIO
    _test_multipart()
  end


  def test_cgi_multipart_tempfile
    @boundary = '----WebKitFormBoundaryAAfvAII+YL9102cX'
    @data = [
      {:name=>'hidden1', :value=>'foobar'},
      {:name=>'text1',   :value=>"\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\xE3\x81\x88\xE3\x81\x8A"},
      {:name=>'file1',   :value=>_read('file1.html'),
       :filename=>'file1.html', :content_type=>'text/html'},
      {:name=>'image1',  :value=>_read('large.png'),
       :filename=>'large.png',  :content_type=>'image/png'},  # large image
    ]
    @data[1][:value].force_encoding(::Encoding::UTF_8) if defined?(::Encoding)
    @expected_class = Tempfile
    _test_multipart()
  end


  def _set_const(klass, name, value)
    old = nil
    klass.class_eval do
      old = const_get(name)
      remove_const(name)
      const_set(name, value)
    end
    return old
  end


  def test_cgi_multipart_maxmultipartlength
    @data = [
      {:name=>'image1', :value=>_read('large.png'),
       :filename=>'large.png', :content_type=>'image/png'},  # large image
    ]
    original = _set_const(CGI, :MAX_MULTIPART_LENGTH, 2 * 1024)
    begin
      ex = assert_raise(StandardError) do
        _test_multipart()
      end
      assert_equal("too large multipart data.", ex.message)
    ensure
      _set_const(CGI, :MAX_MULTIPART_LENGTH, original)
    end
  end if CGI.const_defined?(:MAX_MULTIPART_LENGTH)


  def test_cgi_multipart_maxmultipartcount
    @data = [
      {:name=>'file1', :value=>_read('file1.html'),
       :filename=>'file1.html', :content_type=>'text/html'},
    ]
    item = @data.first
    500.times { @data << item }
    #original = _set_const(CGI, :MAX_MULTIPART_COUNT, 128)
    begin
      ex = assert_raise(StandardError) do
        _test_multipart()
      end
      assert_equal("too many parameters.", ex.message)
    ensure
      #_set_const(CGI, :MAX_MULTIPART_COUNT, original)
    end
  end if CGI.const_defined?(:MAX_MULTIPART_COUNT)


  def test_cgi_multipart_badbody   ## [ruby-dev:28470]
    @data = [
      {:name=>'file1', :value=>_read('file1.html'),
       :filename=>'file1.html', :content_type=>'text/html'},
    ]
    _prepare(@data) do |input|
      input2 = input.sub(/--(\r\n)?\z/, "\r\n")
      assert input2 != input
      #p input2
      input2
    end
    ex = assert_raise(EOFError) do
      cgi = RUBY_VERSION>="1.9" ? CGI.new(:accept_charset=>"UTF-8") : CGI.new
    end
    assert_equal("bad content body", ex.message)
    #
    _prepare(@data) do |input|
      input2 = input.sub(/--(\r\n)?\z/, "")
      assert input2 != input
      #p input2
      input2
    end
    ex = assert_raise(EOFError) do
      cgi = RUBY_VERSION>="1.9" ? CGI.new(:accept_charset=>"UTF-8") : CGI.new
    end
    assert_equal("bad content body", ex.message)
  end


  def test_cgi_multipart_quoteboundary  ## [JVN#84798830]
    @boundary = '(.|\n)*'
    @data = [
      {:name=>'hidden1', :value=>'foobar'},
      {:name=>'text1',   :value=>"\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\xE3\x81\x88\xE3\x81\x8A"},
      {:name=>'file1',   :value=>_read('file1.html'),
       :filename=>'file1.html', :content_type=>'text/html'},
      {:name=>'image1',  :value=>_read('small.png'),
       :filename=>'small.png',  :content_type=>'image/png'},  # small image
    ]
    @data[1][:value].force_encoding("UTF-8") if RUBY_VERSION>="1.9"
    _prepare(@data)
    cgi = RUBY_VERSION>="1.9" ? CGI.new(:accept_charset=>"UTF-8") : CGI.new
    assert_equal('file1.html', cgi['file1'].original_filename)
  end

  def test_cgi_multipart_boundary_10240 # [Bug #3866]
    @boundary = 'AaB03x'
    @data = [
      {:name=>'file',   :value=>"b"*10134,
       :filename=>'file.txt', :content_type=>'text/plain'},
      {:name=>'foo',  :value=>"bar"},
    ]
    _prepare(@data)
    cgi = RUBY_VERSION>="1.9" ? CGI.new(:accept_charset=>"UTF-8") : CGI.new
    assert_equal(cgi['foo'], 'bar')
    assert_equal(cgi['file'].read, 'b'*10134)
  end

  ###

  self.instance_methods.each do |method|
    private method if method =~ /^test_(.*)/ && $1 != ENV['TEST']
  end if ENV['TEST']

end
