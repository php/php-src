require "pp"

module DemoApplication
  def initialize(config, enctype)
    super
    @enctype = enctype
  end

  def do_GET(req, res)
    if req.path_info != "/"
      res.set_redirect(WEBrick::HTTPStatus::Found, req.script_name + "/")
    end
    res.body =<<-_end_of_html_
      <HTML>
       <FORM method="POST" enctype=#{@enctype}>
        text: <INPUT type="text" name="text"><BR>
        file: <INPUT type="file" name="file"><BR>
        check:
        <INPUT type="checkbox" name="check" value="a">a,
        <INPUT type="checkbox" name="check" value="b">b,
        <INPUT type="checkbox" name="check" value="c">c,
        <BR>
        <INPUT type="submit">
       </FORM>
      </HTML>
    _end_of_html_
    res['content-type'] = 'text/html; charset=iso-8859-1'
  end

  def do_POST(req, res)
    if req["content-length"].to_i > 1024*10
      raise WEBrick::HTTPStatus::Forbidden, "file size too large"
    end
    res.body =<<-_end_of_html_
      <HTML>
       <H2>Query Parameters</H2>
       #{display_query(req.query)}
       <A href="#{req.path}">return</A>
       <H2>Request</H2>
       <PRE>#{WEBrick::HTMLUtils::escape(PP::pp(req, "", 80))}</PRE>
       <H2>Response</H2>
       <PRE>#{WEBrick::HTMLUtils::escape(PP::pp(res, "", 80))}</PRE>
      </HTML>
    _end_of_html_
    res['content-type'] = 'text/html; charset=iso-8859-1'
  end

  private

  def display_query(q)
    ret = ""
    q.each{|key, val|
      ret << "<H3>#{WEBrick::HTMLUtils::escape(key)}</H3>"
      ret << "<TABLE border=1>"
      ret << make_tr("val", val.inspect)
      ret << make_tr("val.to_a", val.to_a.inspect)
      ret << make_tr("val.to_ary", val.to_ary.inspect)
      ret << "</TABLE>"
    }
    ret
  end

  def make_tr(arg0, arg1)
    "<TR><TD>#{arg0}</TD><TD>#{WEBrick::HTMLUtils::escape(arg1)}</TD></TR>"
  end
end
