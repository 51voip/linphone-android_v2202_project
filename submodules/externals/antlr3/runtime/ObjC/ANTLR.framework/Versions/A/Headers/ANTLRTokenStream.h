// [The "BSD licence"]
// Copyright (c) 2006-2007 Kay Roepke 2010 Alan Condit
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#import "ANTLRIntStream.h"
#import "ANTLRToken.h"

@protocol ANTLRTokenStream < ANTLRIntStream >

// Get Token at current input pointer + i ahead where i=1 is next Token.
// i<0 indicates tokens in the past.  So -1 is previous token and -2 is
// two tokens ago. LT:0 is undefined.  For i>=n, return Token.EOFToken.
// Return null for LT:0 and any index that results in an absolute address
// that is negative.

- (id<ANTLRToken>) LT:(NSInteger) i;

- (id<ANTLRToken>) getToken:(NSUInteger) i;

- (id) getTokenSource;

- (NSString *) toString;
/** Return the text of all tokens from start to stop, inclusive.
 *  If the stream does not buffer all the tokens then it can just
 *  return "" or null;  Users should not access $ruleLabel.text in
 *  an action of course in that case.
 */
- (NSString *)toStringFromStart:(NSInteger)startIdx ToEnd:(NSInteger)stopIdx;

/** Because the user is not required to use a token with an index stored
 *  in it, we must provide a means for two token objects themselves to
 *  indicate the start/end location.  Most often this will just delegate
 *  to the other toString(int,int).  This is also parallel with
 *  the TreeNodeStream.toString(Object,Object).
 */
- (NSString *) toStringFromToken:(id<ANTLRToken>)startToken ToToken:(id<ANTLRToken>)stopToken;


@end
