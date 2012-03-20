<stylesheet	xmlns="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:var="http://jaxen.org/test-harness/var">
<!-- this is what I used to generate XPathTestBase. After generating I fixed the illegal strings (its quicker
than fixing the xsl for that few errors) and reformatted the code. Its unlikely this code will be needed
again, its just in cvs for completeness -->
    <output method="text"/>
    <template match="/">
        <text>
            /*
 * $Header: /home/projects/jaxen/scm/jaxen/src/java/test/org/jaxen/XPathTestBase.java,v 1.32 2005/06/15 23:52:40 bewins Exp $
 * $Revision: 1.32 $
 * $Date: 2005/06/15 23:52:40 $
 *
 * ====================================================================
 *
 * Copyright (C) 2000-2002 bob mcwhirter &amp; James Strachan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions, and the disclaimer that follows
 *    these conditions in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. The name "Jaxen" must not be used to endorse or promote products
 *    derived from this software without prior written permission.  For
 *    written permission, please contact license@jaxen.org.
 *
 * 4. Products derived from this software may not be called "Jaxen", nor
 *    may "Jaxen" appear in their name, without prior written permission
 *    from the Jaxen Project Management (pm@jaxen.org).
 *
 * In addition, we request (but do not require) that you include in the
 * end-user documentation provided with the redistribution and/or in the
 * software itself an acknowledgement equivalent to the following:
 *     "This product includes software developed by the
 *      Jaxen Project (http://www.jaxen.org/)."
 * Alternatively, the acknowledgment may be graphical using the logos
 * available at http://www.jaxen.org/
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE Jaxen AUTHORS OR THE PROJECT
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ====================================================================
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Jaxen Project and was originally
 * created by bob mcwhirter &lt;bob@werken.com> and
 * James Strachan &lt;jstrachan@apache.org>.  For more information on the
 * Jaxen Project, please see &lt;http://www.jaxen.org/>.
 *
 * $Id: XPathTestBase.java,v 1.32 2005/06/15 23:52:40 bewins Exp $
 */


package org.jaxen;

import junit.framework.TestCase;
import org.jaxen.function.StringFunction;
import org.jaxen.saxpath.helpers.XPathReaderFactory;

import javax.xml.parsers.ParserConfigurationException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public abstract class XPathTestBase extends TestCase
{
    protected static String VAR_URI   = "http://jaxen.org/test-harness/var";
    protected static String TESTS_XML = "xml/test/tests.xml";

    protected static boolean verbose = true;
    protected static boolean debug = true;
    private ContextSupport  contextSupport;

    public XPathTestBase(String name)
    {
        super( name );
    }

    public void setUp() throws ParserConfigurationException
    {
        this.contextSupport = null;
        System.setProperty( XPathReaderFactory.DRIVER_PROPERTY,
                            "" );
        log( "-----------------------------" );
    }

    public void log(String text)
    {
        log( verbose,
             text );
    }

    public void log(boolean actualVerbose,
                    String text)
    {
        if ( ! actualVerbose )
        {
            return;
        }

        System.out.println( text );
    }

    protected void assertCountXPath(int expectedSize, Object context, String xpathStr) throws JaxenException {
        try
        {
            assertCountXPath2(expectedSize, context, xpathStr);
        }
        catch (UnsupportedAxisException e)
        {
            log ( debug,
                  "      ## SKIPPED -- Unsupported Axis" );
        }
    }

    protected Object assertCountXPath2(int expectedSize, Object context, String xpathStr) throws JaxenException {
        log ( debug,
              "  Select :: " + xpathStr );
        BaseXPath xpath = new BaseXPath( xpathStr );
        List results = xpath.selectNodes( getContext( context ) );
        log ( debug,
              "    Expected Size :: " + expectedSize );
        log ( debug,
              "    Result Size   :: " + results.size() );

        if ( expectedSize != results.size() )
        {
            log ( debug,
                  "      ## FAILED" );
            log ( debug,
                  "      ## xpath: " + xpath + " = " + xpath.debug() );

            Iterator resultIter = results.iterator();

            while ( resultIter.hasNext() )
            {
                log ( debug,
                      "      --> " + resultIter.next() );
            }
        }
        assertEquals( xpathStr,
                      expectedSize,
                      results.size() );
        if (expectedSize > 0) {
            return results.get(0);
        }
        return null;
    }

    protected void assertInvalidXPath(Object context, String xpathStr) throws JaxenException {
        try 
        {
            log ( debug,
                  "  Select :: " + xpathStr );
            BaseXPath xpath = new BaseXPath( xpathStr );
            List results = xpath.selectNodes( getContext( context ) );
            log ( debug,
                  "    Result Size   :: " + results.size() );
            fail("An exception was expected.");
        }
        catch (UnsupportedAxisException e)
        {
            log ( debug,
                  "      ## SKIPPED -- Unsupported Axis" );
        }
        catch (JaxenException e) {
            log (debug, "    Caught expected exception "+e.getMessage());
        }
    }

    protected void assertValueOfXPath(String expected, Object context, String xpathStr) throws JaxenException {
        try
        {
            BaseXPath xpath = new BaseXPath( xpathStr );
            Object node = xpath.evaluate( getContext( context ) );
            
            String result = StringFunction.evaluate( node,
                                                     getNavigator() );
            
            log ( debug,
                  "  Select :: " + xpathStr );
            log ( debug,
                  "    Expected :: " + expected );
            log ( debug,
                  "    Result   :: " + result );
            
            if ( ! expected.equals( result ) )
            {
                log ( debug,
                      "      ## FAILED" );
                log ( debug,
                      "      ## xpath: " + xpath + " = " + xpath.debug() );
            }
            
            assertEquals( xpathStr,
                          expected,
                          result );
        }
        catch (UnsupportedAxisException e)
        {
            log ( debug,
                  "      ## SKIPPED -- Unsupported Axis " );

        }        
    }
    protected Context getContext(Object contextNode)
    {
        Context context = new Context( getContextSupport() );

        List list = new ArrayList( 1 );
        list.add( contextNode );
        context.setNodeSet( list );

        return context;
    }

    public ContextSupport getContextSupport()
    {
        if ( this.contextSupport == null )
        {
            this.contextSupport = new ContextSupport( new SimpleNamespaceContext(),
                                                      XPathFunctionContext.getInstance(),
                                                      new SimpleVariableContext(),
                                                      getNavigator() );
        }

        return this.contextSupport;
    }

    public abstract Navigator getNavigator();

    public abstract Object getDocument(String url) throws Exception;

    public void testGetNodeType() throws FunctionCallException, UnsupportedAxisException
    {
        Navigator nav = getNavigator();
        Object document = nav.getDocument("xml/testNamespaces.xml");
        int count = 0;
        Iterator descendantOrSelfAxisIterator = nav.getDescendantOrSelfAxisIterator(document);
        while (descendantOrSelfAxisIterator.hasNext()) {
            Object node = descendantOrSelfAxisIterator.next();
            Iterator namespaceAxisIterator = nav.getNamespaceAxisIterator(node);
            while (namespaceAxisIterator.hasNext()) {
                count++;
                assertEquals("Node type mismatch", Pattern.NAMESPACE_NODE, nav.getNodeType(namespaceAxisIterator.next()));
            }
        }
        assertEquals(25, count);
    }

        </text>
        <apply-templates select="node()|@*"/>
        <text>
}            
        </text>
    </template>
    <template match="context">
        <text>
    public void test</text><value-of select="generate-id()"/><text>() throws JaxenException {
        Navigator nav = getNavigator();
        String url = "</text><value-of select="../@url"/><text>";
        log( "Document [" + url + "]" );
        Object document = nav.getDocument(url);

        XPath contextpath = new BaseXPath("</text><value-of select="@select"/><text>", nav);
        log( "Initial Context :: " + contextpath );
        List list = contextpath.selectNodes(document);
        </text>
        <if test="count(namespace::*) > count(../namespace::*)">
        <text>
        SimpleNamespaceContext nsContext = new SimpleNamespaceContext();</text>
            <for-each select="namespace::*[local-name() != 'var' and local-name() != 'xml']">
                <text>
        nsContext.addNamespace( "</text><value-of select="local-name()"/><text>", "</text><value-of select="."/><text>" );</text>
           </for-each>
            <text>
        getContextSupport().setNamespaceContext( nsContext );</text>
        </if>
        <if test="@*[namespace-uri() = 'http://jaxen.org/test-harness/var']">
        <text>
        SimpleVariableContext varContext = new SimpleVariableContext();</text>
            <for-each select="@*[namespace-uri() = 'http://jaxen.org/test-harness/var']">
                <text>
        varContext.setVariableValue(null, "</text><value-of select="local-name()"/><text>", "</text><value-of select="."/><text>" );</text>
           </for-each>
            <text>
        getContextSupport().setVariableContext( varContext );</text>
        </if>
        <text>
        Iterator iter = list.iterator();
        while (iter.hasNext()) {
            Object context = iter.next();</text>
            <apply-templates select="node()|@*"/>
        <text>
        }
    }</text>
    </template>
    <template match="test[@exception]">
        <text>
            assertInvalidXPath(context, "</text><value-of select='@select'/><text>");</text>
    </template>
    <template match="test[valueOf]">
        <choose>
            <when test="@count">
                <text>
            try
            {
                Object result = assertCountXPath2(</text><value-of select="@count"/><text>, context, "</text><value-of select="@select"/><text>");</text>
        <for-each select="valueOf">
            <text>
                assertValueOfXPath("</text><value-of select="."/><text>", result, "</text><value-of select="@select"/><text>");</text>
        </for-each>
        <text>
            }
            catch (UnsupportedAxisException e)
            {
                log ( debug, "      ## SKIPPED -- Unsupported Axis" );
            }</text>
            </when>
            <otherwise>
                <text>
            try
            {
                BaseXPath xpath = new BaseXPath( "</text><value-of select="@select"/><text>" );
                List results = xpath.selectNodes( getContext( context ) );
                Object result = results.get(0);</text>
        <for-each select="valueOf">
            <text>
                assertValueOfXPath("</text><value-of select="."/><text>", result, "</text><value-of select="@select"/><text>");</text>
        </for-each>
        <text>
            }
            catch (UnsupportedAxisException e)
            {
                log ( debug, "      ## SKIPPED -- Unsupported Axis" );
            }</text>
            </otherwise>
        </choose>
    </template>
    <template match="test">
        <text>
            assertCountXPath(</text><value-of select="@count"/><text>, context, "</text><value-of select="@select"/><text>");</text>
    </template>
    <template match="valueOf">
        <text>
            assertValueOfXPath("</text><value-of select="."/>", context, "<value-of select="@select"/><text>");</text>
    </template>
    <template match="comment()"><text>
    /*</text><value-of select="."/><text>
    */</text>
    </template>
    <template match="node()|@*"><apply-templates select="node()|@*"/></template>    
</stylesheet>