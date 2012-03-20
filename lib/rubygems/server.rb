require 'webrick'
require 'zlib'
require 'erb'

require 'rubygems'
require 'rubygems/doc_manager'

##
# Gem::Server and allows users to serve gems for consumption by
# `gem --remote-install`.
#
# gem_server starts an HTTP server on the given port and serves the following:
# * "/" - Browsing of gem spec files for installed gems
# * "/specs.#{Gem.marshal_version}.gz" - specs name/version/platform index
# * "/latest_specs.#{Gem.marshal_version}.gz" - latest specs
#   name/version/platform index
# * "/quick/" - Individual gemspecs
# * "/gems" - Direct access to download the installable gems
# * "/rdoc?q=" - Search for installed rdoc documentation
# * legacy indexes:
#   * "/Marshal.#{Gem.marshal_version}" - Full SourceIndex dump of metadata
#     for installed gems
#
# == Usage
#
#   gem_server = Gem::Server.new Gem.dir, 8089, false
#   gem_server.run
#
#--
# TODO Refactor into a real WEBrick servlet to remove code duplication.

class Gem::Server

  attr_reader :spec_dirs

  include ERB::Util
  include Gem::UserInteraction

  SEARCH = <<-SEARCH
      <form class="headerSearch" name="headerSearchForm" method="get" action="/rdoc">
        <div id="search" style="float:right">
          <label for="q">Filter/Search</label>
          <input id="q" type="text" style="width:10em" name="q">
          <button type="submit" style="display:none"></button>
        </div>
      </form>
  SEARCH

  DOC_TEMPLATE = <<-'DOC_TEMPLATE'
  <?xml version="1.0" encoding="iso-8859-1"?>
  <!DOCTYPE html
       PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

  <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <title>RubyGems Documentation Index</title>
    <link rel="stylesheet" href="gem-server-rdoc-style.css" type="text/css" media="screen" />
  </head>
  <body>
    <div id="fileHeader">
<%= SEARCH %>
      <h1>RubyGems Documentation Index</h1>
    </div>
    <!-- banner header -->

  <div id="bodyContent">
    <div id="contextContent">
      <div id="description">
        <h1>Summary</h1>
  <p>There are <%=values["gem_count"]%> gems installed:</p>
  <p>
  <%= values["specs"].map { |v| "<a href=\"##{v["name"]}\">#{v["name"]}</a>" }.join ', ' %>.
  <h1>Gems</h1>

  <dl>
  <% values["specs"].each do |spec| %>
    <dt>
    <% if spec["first_name_entry"] then %>
      <a name="<%=spec["name"]%>"></a>
    <% end %>

    <b><%=spec["name"]%> <%=spec["version"]%></b>

    <% if spec["rdoc_installed"] then %>
      <a href="<%=spec["doc_path"]%>">[rdoc]</a>
    <% else %>
      <span title="rdoc not installed">[rdoc]</span>
    <% end %>

    <% if spec["homepage"] then %>
      <a href="<%=spec["homepage"]%>" title="<%=spec["homepage"]%>">[www]</a>
    <% else %>
      <span title="no homepage available">[www]</span>
    <% end %>

    <% if spec["has_deps"] then %>
     - depends on
      <%= spec["dependencies"].map { |v| "<a href=\"##{v["name"]}\">#{v["name"]}</a>" }.join ', ' %>.
    <% end %>
    </dt>
    <dd>
    <%=spec["summary"]%>
    <% if spec["executables"] then %>
      <br/>

      <% if spec["only_one_executable"] then %>
          Executable is
      <% else %>
          Executables are
      <%end%>

      <%= spec["executables"].map { |v| "<span class=\"context-item-name\">#{v["executable"]}</span>"}.join ', ' %>.

    <%end%>
    <br/>
    <br/>
    </dd>
  <% end %>
  </dl>

      </div>
     </div>
    </div>
  <div id="validator-badges">
    <p><small><a href="http://validator.w3.org/check/referer">[Validate]</a></small></p>
  </div>
  </body>
  </html>
  DOC_TEMPLATE

  # CSS is copy & paste from rdoc-style.css, RDoc V1.0.1 - 20041108
  RDOC_CSS = <<-RDOC_CSS
body {
    font-family: Verdana,Arial,Helvetica,sans-serif;
    font-size:   90%;
    margin: 0;
    margin-left: 40px;
    padding: 0;
    background: white;
}

h1,h2,h3,h4 { margin: 0; color: #efefef; background: transparent; }
h1 { font-size: 150%; }
h2,h3,h4 { margin-top: 1em; }

a { background: #eef; color: #039; text-decoration: none; }
a:hover { background: #039; color: #eef; }

/* Override the base stylesheets Anchor inside a table cell */
td > a {
  background: transparent;
  color: #039;
  text-decoration: none;
}

/* and inside a section title */
.section-title > a {
  background: transparent;
  color: #eee;
  text-decoration: none;
}

/* === Structural elements =================================== */

div#index {
    margin: 0;
    margin-left: -40px;
    padding: 0;
    font-size: 90%;
}


div#index a {
    margin-left: 0.7em;
}

div#index .section-bar {
   margin-left: 0px;
   padding-left: 0.7em;
   background: #ccc;
   font-size: small;
}


div#classHeader, div#fileHeader {
    width: auto;
    color: white;
    padding: 0.5em 1.5em 0.5em 1.5em;
    margin: 0;
    margin-left: -40px;
    border-bottom: 3px solid #006;
}

div#classHeader a, div#fileHeader a {
    background: inherit;
    color: white;
}

div#classHeader td, div#fileHeader td {
    background: inherit;
    color: white;
}


div#fileHeader {
    background: #057;
}

div#classHeader {
    background: #048;
}


.class-name-in-header {
  font-size:  180%;
  font-weight: bold;
}


div#bodyContent {
    padding: 0 1.5em 0 1.5em;
}

div#description {
    padding: 0.5em 1.5em;
    background: #efefef;
    border: 1px dotted #999;
}

div#description h1,h2,h3,h4,h5,h6 {
    color: #125;;
    background: transparent;
}

div#validator-badges {
    text-align: center;
}
div#validator-badges img { border: 0; }

div#copyright {
    color: #333;
    background: #efefef;
    font: 0.75em sans-serif;
    margin-top: 5em;
    margin-bottom: 0;
    padding: 0.5em 2em;
}


/* === Classes =================================== */

table.header-table {
    color: white;
    font-size: small;
}

.type-note {
    font-size: small;
    color: #DEDEDE;
}

.xxsection-bar {
    background: #eee;
    color: #333;
    padding: 3px;
}

.section-bar {
   color: #333;
   border-bottom: 1px solid #999;
    margin-left: -20px;
}


.section-title {
    background: #79a;
    color: #eee;
    padding: 3px;
    margin-top: 2em;
    margin-left: -30px;
    border: 1px solid #999;
}

.top-aligned-row {  vertical-align: top }
.bottom-aligned-row { vertical-align: bottom }

/* --- Context section classes ----------------------- */

.context-row { }
.context-item-name { font-family: monospace; font-weight: bold; color: black; }
.context-item-value { font-size: small; color: #448; }
.context-item-desc { color: #333; padding-left: 2em; }

/* --- Method classes -------------------------- */
.method-detail {
    background: #efefef;
    padding: 0;
    margin-top: 0.5em;
    margin-bottom: 1em;
    border: 1px dotted #ccc;
}
.method-heading {
  color: black;
  background: #ccc;
  border-bottom: 1px solid #666;
  padding: 0.2em 0.5em 0 0.5em;
}
.method-signature { color: black; background: inherit; }
.method-name { font-weight: bold; }
.method-args { font-style: italic; }
.method-description { padding: 0 0.5em 0 0.5em; }

/* --- Source code sections -------------------- */

a.source-toggle { font-size: 90%; }
div.method-source-code {
    background: #262626;
    color: #ffdead;
    margin: 1em;
    padding: 0.5em;
    border: 1px dashed #999;
    overflow: hidden;
}

div.method-source-code pre { color: #ffdead; overflow: hidden; }

/* --- Ruby keyword styles --------------------- */

.standalone-code { background: #221111; color: #ffdead; overflow: hidden; }

.ruby-constant  { color: #7fffd4; background: transparent; }
.ruby-keyword { color: #00ffff; background: transparent; }
.ruby-ivar    { color: #eedd82; background: transparent; }
.ruby-operator  { color: #00ffee; background: transparent; }
.ruby-identifier { color: #ffdead; background: transparent; }
.ruby-node    { color: #ffa07a; background: transparent; }
.ruby-comment { color: #b22222; font-weight: bold; background: transparent; }
.ruby-regexp  { color: #ffa07a; background: transparent; }
.ruby-value   { color: #7fffd4; background: transparent; }
  RDOC_CSS

  RDOC_NO_DOCUMENTATION = <<-'NO_DOC'
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
          "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <title>Found documentation</title>
    <link rel="stylesheet" href="gem-server-rdoc-style.css" type="text/css" media="screen" />
  </head>
  <body>
    <div id="fileHeader">
<%= SEARCH %>
      <h1>No documentation found</h1>
    </div>

    <div id="bodyContent">
      <div id="contextContent">
        <div id="description">
          <p>No gems matched <%= h query.inspect %></p>

          <p>
            Back to <a href="/">complete gem index</a>
          </p>

        </div>
      </div>
    </div>
    <div id="validator-badges">
      <p><small><a href="http://validator.w3.org/check/referer">[Validate]</a></small></p>
    </div>
  </body>
</html>
  NO_DOC

  RDOC_SEARCH_TEMPLATE = <<-'RDOC_SEARCH'
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
          "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <title>Found documentation</title>
    <link rel="stylesheet" href="gem-server-rdoc-style.css" type="text/css" media="screen" />
  </head>
  <body>
    <div id="fileHeader">
<%= SEARCH %>
      <h1>Found documentation</h1>
    </div>
    <!-- banner header -->

    <div id="bodyContent">
      <div id="contextContent">
        <div id="description">
          <h1>Summary</h1>
          <p><%=doc_items.length%> documentation topics found.</p>
          <h1>Topics</h1>

          <dl>
          <% doc_items.each do |doc_item| %>
            <dt>
              <b><%=doc_item[:name]%></b>
              <a href="<%=doc_item[:url]%>">[rdoc]</a>
            </dt>
            <dd>
              <%=doc_item[:summary]%>
              <br/>
              <br/>
            </dd>
          <% end %>
          </dl>

          <p>
            Back to <a href="/">complete gem index</a>
          </p>

        </div>
      </div>
    </div>
    <div id="validator-badges">
      <p><small><a href="http://validator.w3.org/check/referer">[Validate]</a></small></p>
    </div>
  </body>
</html>
  RDOC_SEARCH

  def self.run(options)
    new(options[:gemdir], options[:port], options[:daemon],
        options[:launch], options[:addresses]).run
  end

  ##
  # Only the first directory in gem_dirs is used for serving gems

  def initialize(gem_dirs, port, daemon, launch = nil, addresses = nil)
    Socket.do_not_reverse_lookup = true

    @gem_dirs = Array gem_dirs
    @port = port
    @daemon = daemon
    @launch = launch
    @addresses = addresses
    logger = WEBrick::Log.new nil, WEBrick::BasicLog::FATAL
    @server = WEBrick::HTTPServer.new :DoNotListen => true, :Logger => logger

    @spec_dirs = @gem_dirs.map do |gem_dir|
      spec_dir = File.join gem_dir, 'specifications'

      unless File.directory? spec_dir then
        raise ArgumentError, "#{gem_dir} does not appear to be a gem repository"
      end

      spec_dir
    end

    Gem::Specification.dirs = @gem_dirs
  end

  def Marshal(req, res)
    Gem::Specification.reset

    add_date res

    index = Gem::Deprecate.skip_during { Marshal.dump Gem.source_index }

    if req.request_method == 'HEAD' then
      res['content-length'] = index.length
      return
    end

    if req.path =~ /Z$/ then
      res['content-type'] = 'application/x-deflate'
      index = Gem.deflate index
    else
      res['content-type'] = 'application/octet-stream'
    end

    res.body << index
  end

  def add_date res
    res['date'] = @spec_dirs.map do |spec_dir|
      File.stat(spec_dir).mtime
    end.max
  end

  def latest_specs(req, res)
    Gem::Specification.reset

    res['content-type'] = 'application/x-gzip'

    add_date res

    latest_specs = Gem::Specification.latest_specs

    specs = latest_specs.sort.map do |spec|
      platform = spec.original_platform || Gem::Platform::RUBY
      [spec.name, spec.version, platform]
    end

    specs = Marshal.dump specs

    if req.path =~ /\.gz$/ then
      specs = Gem.gzip specs
      res['content-type'] = 'application/x-gzip'
    else
      res['content-type'] = 'application/octet-stream'
    end

    if req.request_method == 'HEAD' then
      res['content-length'] = specs.length
    else
      res.body << specs
    end
  end

  ##
  # Creates server sockets based on the addresses option.  If no addresses
  # were given a server socket for all interfaces is created.

  def listen addresses = @addresses
    addresses = [nil] unless addresses

    listeners = 0

    addresses.each do |address|
      begin
        @server.listen address, @port
        @server.listeners[listeners..-1].each do |listener|
          host, port = listener.addr.values_at 2, 1
          host = "[#{host}]" if host =~ /:/ # we don't reverse lookup
          say "Server started at http://#{host}:#{port}"
        end

        listeners = @server.listeners.length
      rescue SystemCallError
        next
      end
    end

    if @server.listeners.empty? then
      say "Unable to start a server."
      say "Check for running servers or your --bind and --port arguments"
      terminate_interaction 1
    end
  end

  def quick(req, res)
    Gem::Specification.reset

    res['content-type'] = 'text/plain'
    add_date res

    case req.request_uri.path
    when %r|^/quick/(Marshal.#{Regexp.escape Gem.marshal_version}/)?(.*?)-([0-9.]+)(-.*?)?\.gemspec\.rz$| then
      marshal_format, name, version, platform = $1, $2, $3, $4
      specs = Gem::Specification.find_all_by_name name, version

      selector = [name, version, platform].map(&:inspect).join ' '

      platform = if platform then
                   Gem::Platform.new platform.sub(/^-/, '')
                 else
                   Gem::Platform::RUBY
                 end

      specs = specs.select { |s| s.platform == platform }

      if specs.empty? then
        res.status = 404
        res.body = "No gems found matching #{selector}"
      elsif specs.length > 1 then
        res.status = 500
        res.body = "Multiple gems found matching #{selector}"
      elsif marshal_format then
        res['content-type'] = 'application/x-deflate'
        res.body << Gem.deflate(Marshal.dump(specs.first))
      end
    else
      raise WEBrick::HTTPStatus::NotFound, "`#{req.path}' not found."
    end
  end

  def root(req, res)
    Gem::Specification.reset
    add_date res

    raise WEBrick::HTTPStatus::NotFound, "`#{req.path}' not found." unless
      req.path == '/'

    specs = []
    total_file_count = 0

    Gem::Specification.each do |spec|
      total_file_count += spec.files.size
      deps = spec.dependencies.map { |dep|
        {
          "name"    => dep.name,
          "type"    => dep.type,
          "version" => dep.requirement.to_s,
        }
      }

      deps = deps.sort_by { |dep| [dep["name"].downcase, dep["version"]] }
      deps.last["is_last"] = true unless deps.empty?

      # executables
      executables = spec.executables.sort.collect { |exec| {"executable" => exec} }
      executables = nil if executables.empty?
      executables.last["is_last"] = true if executables

      specs << {
        "authors"             => spec.authors.sort.join(", "),
        "date"                => spec.date.to_s,
        "dependencies"        => deps,
        "doc_path"            => "/doc_root/#{spec.full_name}/rdoc/index.html",
        "executables"         => executables,
        "only_one_executable" => (executables && executables.size == 1),
        "full_name"           => spec.full_name,
        "has_deps"            => !deps.empty?,
        "homepage"            => spec.homepage,
        "name"                => spec.name,
        "rdoc_installed"      => Gem::DocManager.new(spec).rdoc_installed?,
        "summary"             => spec.summary,
        "version"             => spec.version.to_s,
      }
    end

    specs << {
      "authors" => "Chad Fowler, Rich Kilmer, Jim Weirich, Eric Hodel and others",
      "dependencies" => [],
      "doc_path" => "/doc_root/rubygems-#{Gem::VERSION}/rdoc/index.html",
      "executables" => [{"executable" => 'gem', "is_last" => true}],
      "only_one_executable" => true,
      "full_name" => "rubygems-#{Gem::VERSION}",
      "has_deps" => false,
      "homepage" => "http://docs.rubygems.org/",
      "name" => 'rubygems',
      "rdoc_installed" => true,
      "summary" => "RubyGems itself",
      "version" => Gem::VERSION,
    }

    specs = specs.sort_by { |spec| [spec["name"].downcase, spec["version"]] }
    specs.last["is_last"] = true

    # tag all specs with first_name_entry
    last_spec = nil
    specs.each do |spec|
      is_first = last_spec.nil? || (last_spec["name"].downcase != spec["name"].downcase)
      spec["first_name_entry"] = is_first
      last_spec = spec
    end

    # create page from template
    template = ERB.new(DOC_TEMPLATE)
    res['content-type'] = 'text/html'

    values = { "gem_count" => specs.size.to_s, "specs" => specs,
               "total_file_count" => total_file_count.to_s }

    # suppress 1.9.3dev warning about unused variable
    values = values

    result = template.result binding
    res.body = result
  end

  ##
  # Can be used for quick navigation to the rdoc documentation.  You can then
  # define a search shortcut for your browser.  E.g. in Firefox connect
  # 'shortcut:rdoc' to http://localhost:8808/rdoc?q=%s template. Then you can
  # directly open the ActionPack documentation by typing 'rdoc actionp'. If
  # there are multiple hits for the search term, they are presented as a list
  # with links.
  #
  # Search algorithm aims for an intuitive search:
  # 1. first try to find the gems and documentation folders which name
  #    starts with the search term
  # 2. search for entries, that *contain* the search term
  # 3. show all the gems
  #
  # If there is only one search hit, user is immediately redirected to the
  # documentation for the particular gem, otherwise a list with results is
  # shown.
  #
  # === Additional trick - install documentation for ruby core
  #
  # Note: please adjust paths accordingly use for example 'locate yaml.rb' and
  # 'gem environment' to identify directories, that are specific for your
  # local installation
  #
  # 1. install ruby sources
  #      cd /usr/src
  #      sudo apt-get source ruby
  #
  # 2. generate documentation
  #      rdoc -o /usr/lib/ruby/gems/1.8/doc/core/rdoc \
  #        /usr/lib/ruby/1.8 ruby1.8-1.8.7.72
  #
  # By typing 'rdoc core' you can now access the core documentation

  def rdoc(req, res)
    query = req.query['q']
    show_rdoc_for_pattern("#{query}*", res) && return
    show_rdoc_for_pattern("*#{query}*", res) && return

    template = ERB.new RDOC_NO_DOCUMENTATION

    res['content-type'] = 'text/html'
    res.body = template.result binding
  end

  ##
  # Returns true and prepares http response, if rdoc for the requested gem
  # name pattern was found.
  #
  # The search is based on the file system content, not on the gems metadata.
  # This allows additional documentation folders like 'core' for the ruby core
  # documentation - just put it underneath the main doc folder.

  def show_rdoc_for_pattern(pattern, res)
    found_gems = Dir.glob("{#{@gem_dirs.join ','}}/doc/#{pattern}").select {|path|
      File.exist? File.join(path, 'rdoc/index.html')
    }
    case found_gems.length
    when 0
      return false
    when 1
      new_path = File.basename(found_gems[0])
      res.status = 302
      res['Location'] = "/doc_root/#{new_path}/rdoc/index.html"
      return true
    else
      doc_items = []
      found_gems.each do |file_name|
        base_name = File.basename(file_name)
        doc_items << {
          :name => base_name,
          :url => "/doc_root/#{base_name}/rdoc/index.html",
          :summary => ''
        }
      end

      template = ERB.new(RDOC_SEARCH_TEMPLATE)
      res['content-type'] = 'text/html'
      result = template.result binding
      res.body = result
      return true
    end
  end

  def run
    listen

    WEBrick::Daemon.start if @daemon

    @server.mount_proc "/Marshal.#{Gem.marshal_version}", method(:Marshal)
    @server.mount_proc "/Marshal.#{Gem.marshal_version}.Z", method(:Marshal)

    @server.mount_proc "/specs.#{Gem.marshal_version}", method(:specs)
    @server.mount_proc "/specs.#{Gem.marshal_version}.gz", method(:specs)

    @server.mount_proc "/latest_specs.#{Gem.marshal_version}",
                       method(:latest_specs)
    @server.mount_proc "/latest_specs.#{Gem.marshal_version}.gz",
                       method(:latest_specs)

    @server.mount_proc "/quick/", method(:quick)

    @server.mount_proc("/gem-server-rdoc-style.css") do |req, res|
      res['content-type'] = 'text/css'
      add_date res
      res.body << RDOC_CSS
    end

    @server.mount_proc "/", method(:root)

    @server.mount_proc "/rdoc", method(:rdoc)

    paths = { "/gems" => "/cache/", "/doc_root" => "/doc/" }
    paths.each do |mount_point, mount_dir|
      @server.mount(mount_point, WEBrick::HTTPServlet::FileHandler,
                    File.join(@gem_dirs.first, mount_dir), true)
    end

    trap("INT") { @server.shutdown; exit! }
    trap("TERM") { @server.shutdown; exit! }

    launch if @launch

    @server.start
  end

  def specs(req, res)
    Gem::Specification.reset

    add_date res

    specs = Gem::Specification.sort_by(&:sort_obj).map do |spec|
      platform = spec.original_platform || Gem::Platform::RUBY
      [spec.name, spec.version, platform]
    end

    specs = Marshal.dump specs

    if req.path =~ /\.gz$/ then
      specs = Gem.gzip specs
      res['content-type'] = 'application/x-gzip'
    else
      res['content-type'] = 'application/octet-stream'
    end

    if req.request_method == 'HEAD' then
      res['content-length'] = specs.length
    else
      res.body << specs
    end
  end

  def launch
    listeners = @server.listeners.map{|l| l.addr[2] }

    # TODO: 0.0.0.0 == any, not localhost.
    host = listeners.any?{|l| l == '0.0.0.0'} ? 'localhost' : listeners.first

    say "Launching browser to http://#{host}:#{@port}"

    system("#{@launch} http://#{host}:#{@port}")
  end
end
