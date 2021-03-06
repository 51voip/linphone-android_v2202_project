#!/usr/bin/ruby
# encoding: utf-8

=begin LICENSE

[The "BSD licence"]
Copyright (c) 2009-2010 Kyle Yetter
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=end

=begin rdoc ANTLR3

The main namespace for the ANTLR runtime libraries, which are used by
Ruby-targeted recognizers generated by ANTLR. The entire library is segmented
into several main components, as well as a few additional utility components,
each contained within a separate script.

== Library Components

Not all components of the ANTLR3 library are necessary within ANTLR generated
code. Some components are only used within specific types of recognizers and
some are simply extra utilities for use by anyone working with ANTLR code. Thus,
when requiring 'antlr3', only the essential core components are loaded
immediately. The rest are configured to autoload when any of the constant names
they define are referenced.

The following list gives a brief introduction to each component of the ANTLR3
runtime library. The items are loosely ordered by importance.

antlr3/recognizers.rb::
  contains the base classes for ANTLR-generated recognizers, and thus, is one of
  the most important components of the runtime library. loaded by default
antlr3/dfa.rb::
  defines a single DFA class that is used to simulate state machines for certain
  decisions recognizers must make in code generated by ANTLR
antlr3/streams.rb::
  defines the stream classes used by ANTLR recognizers to walk sequentially
  through strings, tokens, and tree nodes loaded by default
antlr3/token.rb::
  contains all modules and classes concerned with making tokens, the chunks of
  text and symbol information produced by lexers and used by parsers and ASTs
  loaded by default
antlr3/error.rb::
  defines the Error module, which contains definitions for most of the many
  error classes used through the runtime library and ANTLR generated
  recognizers. loaded by default
antlr3/constants.rb::
  just a module used as a namespace for the named constant values used
  throughout the library. loaded by default
antlr3/tree.rb::
  contains everything pertaining to Abstract Syntax Trees (ASTs). This script is
  not loaded by default when 'antlr3' is required, but it is autloaded on demand
  when any constant defined in the script is referenced. contents are autoloaded
  on demand
antlr3/debug.rb::
  when code is generated by ANTLR using the '-debug' option, all of the
  additional classes and mixins required by the debug code are contained within
  the Debug module defined by this library. the Debug module is autoloaded on
  demand
antlr3/main.rb::
  defines the Main module. When ANTLR-generated recognizer code is run directly
  as a script (not loaded as a module), the code will behave as a full
  command-line script by using functionality implemented in the Main module. the
  Main module is autloaded on demand
antlr3/tree-wizard.rb::
  contains extra tools to easily construct ASTs by parsing descriptions written
  in a special DSL
antlr3/dot.rb::
  extra utilities to generate DOT map specifications for graphical.
  representations of ASTs

@author Kyle Yetter

=end

module ANTLR3
  
  # :stopdoc:
  # BEGIN PATHS -- do not modify
  
  LIBRARY_PATH  = ::File.expand_path( ::File.dirname( __FILE__ ) ).freeze
  PROJECT_PATH  = ::File.dirname( LIBRARY_PATH ).freeze
  DATA_PATH     = ::File.join( PROJECT_PATH, 'java' ).freeze
  
  # END PATHS
  # :startdoc:
  
  # Returns the library path for the module. If any arguments are given,
  # they will be joined to the end of the libray path using
  # <tt>File.join</tt>.
  #
  def self.library_path( *args )
    ::File.expand_path( ::File.join( LIBRARY_PATH, *args ) )
  end
  
  # Returns the lpath for the module. If any arguments are given,
  # they will be joined to the end of the path using
  # <tt>File.join</tt>.
  #
  def self.data_path( *args )
    ::File.expand_path( ::File.join( DATA_PATH, *args ) )
  end
  
  # This is used internally in a handful of locations in the runtime library
  # where assumptions have been made that a condition will never happen
  # under normal usage conditions and thus an ANTLR3::Bug error will be
  # raised if the condition does occur.
  def self.bug!( message = nil )
    bug = Bug.new( message )
    bug.set_backtrace( caller )
    raise( bug )
  end
  
  @antlr_jar = nil
  
  def self.antlr_jar=( path )
    @antlr_jar = path ? File.expand_path( path.to_s ) : path
  end
  
  def self.antlr_jar
    @antlr_jar and return( @antlr_jar )
    
    path = data_path "antlr-full-#{ ANTLR_VERSION_STRING }.jar"
    if env_path = ENV[ 'RUBY_ANTLR_JAR' ]
      if File.file?( env_path ) then return File.expand_path( env_path ) end
      
      warn( 
        "#{ __FILE__ }:#{ __LINE__ }: " <<
        "ignoring environmental variable RUBY_ANTLR_JAR (=%p) " % env_path <<
        "as it is not the path to an existing file\n" <<
        "  -> trying default jar path %p instead" % path
      )
    end
    
    File.exists?( path ) ? path : nil
  end
  
  ##############################################################################################
  ############################### Namespace and Load Path Setup ################################
  ##############################################################################################
  
  # Tree classes are only used by tree parsers or AST-building parsers
  # Thus, they're not essential for everything ANTLR generates and
  # are autoloaded on-demand
  autoload :AST, 'antlr3/tree'
  
  tree_classes = [ 
    :Tree, :TreeAdaptor, :BaseTree, :BaseTreeAdaptor,
    :CommonTree, :CommonErrorNode, :CommonTreeAdaptor,
    :TreeNodeStream, :CommonTreeNodeStream, :TreeParser,
    :TreeVisitor, :RewriteRuleElementStream,
    :RewriteRuleTokenStream, :RewriteRuleSubtreeStream,
    :RewriteRuleNodeStream
  ]
  
  for klass in tree_classes
    autoload klass, 'antlr3/tree'
  end
  
  # Set up non-essential components to be loaded on-demand
  autoload :TokenRewriteStream, 'antlr3/streams/rewrite'
  autoload :FilterMode, 'antlr3/modes/filter'
  autoload :ASTBuilder, 'antlr3/modes/ast-builder'
  autoload :Main,  'antlr3/main'
  autoload :Debug, 'antlr3/debug'
  autoload :Profile, 'antlr3/profile'
  autoload :DOT, 'antlr3/dot'
  autoload :InteractiveStringStream, 'antlr3/streams/interactive'
  
  autoload :Template, 'antlr3/template'
  
  $LOAD_PATH.include?( library_path ) or $LOAD_PATH.unshift( library_path )
  
end  # module ANTLR3


require 'set'
require 'antlr3/util'
require 'antlr3/version'

unless $0 == 'antlr4ruby'
  require 'antlr3/constants'
  require 'antlr3/error'
  require 'antlr3/token'
  require 'antlr3/recognizers'
  require 'antlr3/dfa'
  require 'antlr3/streams'
end
