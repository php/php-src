/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sam Ruby (rubys@us.ibm.com)                                  |
   +----------------------------------------------------------------------+
*/

package net.php;

import java.io.IOException;
import javax.servlet.*;
import javax.servlet.http.*;

public class servlet extends HttpServlet {

    char slash=System.getProperty("file.separator").charAt(0);
    HttpServletRequest request;
    HttpServletResponse response;
    ServletInputStream stream;

    /******************************************************************/
    /*                          native methods                        */ 
    /******************************************************************/

    static { reflect.loadLibrary("servlet"); }
    native void startup();
    native long define(String name);
    native void send(String requestMethod, String queryString,
      String pathInfo, String pathTranslated,
      String contentType, int contentLength, String authUser);
    native void shutdown();

    /******************************************************************/
    /*                         sapi callbacks                         */ 
    /******************************************************************/

    String readPost(int bytes) {
       try {
         if (stream == null) stream=request.getInputStream();
         byte[] data = new byte[bytes];
         int len = stream.read(data, 0, bytes);
         if (len <= 0) return "";
         return new String(data, 0, len);
       } catch (IOException e) {
         return "";
       }
    }

    String readCookies() {
       reflect.setResult(define("request"), request);
       reflect.setResult(define("response"), response);
       return request.getHeader("cookie");
    }

    void header(String data) {
      try {
        if (data.startsWith("Content-Type: ")) {
          response.setContentType(data.substring(data.indexOf(" ")+1));
        } else if (data.startsWith("Location: ")) {
          response.sendRedirect(data.substring(data.indexOf(" ")+1));
        } else {
          int colon = data.indexOf(": ");
          if (colon > 0) {
            response.addHeader(data.substring(0,colon),
              data.substring(colon+2));
          } else {
            response.getWriter().println(data);
          }
        }
      } catch (IOException e) {
        System.err.print(data);
      }
    }

    void write(String data) {
      try {
        response.getWriter().print(data);
      } catch (IOException e) {
        System.err.print(data);
      }
    }

    /******************************************************************/
    /*                        servlet interface                       */ 
    /******************************************************************/

    public void init(ServletConfig config) throws ServletException {
       super.init(config);
       startup();
    }

    public void service(HttpServletRequest request,
                        HttpServletResponse response) 
       throws ServletException
    {
       this.request=request;
       this.response=response;
       send(request.getMethod(), request.getQueryString(),
            request.getPathInfo(), getPathTranslated(), 
            request.getContentType(), request.getContentLength(),
	    request.getRemoteUser());

       try {
         if (stream != null) stream.close();
       } catch (IOException e) {
         throw new ServletException(e);
       }
    }

    public void destroy() {
      shutdown();
      super.destroy();
    }

    /******************************************************************/
    /*                         utility function                       */ 
    /******************************************************************/

    String getPathTranslated() {
       /* I have no idea why this has to be this hard... */
       String servletPath=request.getServletPath();
       String contextPath=getServletContext().getRealPath(servletPath);
       servletPath=servletPath.replace('/',slash);
       contextPath=contextPath.substring(0,contextPath.lastIndexOf(slash));
       return contextPath+servletPath;
    }
}
