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

#import <Cocoa/Cocoa.h>
#import "ANTLRTokenStream.h"
#import "ANTLRToken.h"
#import "ANTLRCommonToken.h"
#import "ANTLRTokenSource.h"
#import "ANTLRBitSet.h"
#import "ANTLRBufferedTokenStream.h"

@interface ANTLRCommonTokenStream : ANTLRBufferedTokenStream < ANTLRTokenStream >
{
	NSMutableDictionary *channelOverride;
	NSInteger channel;
}

@property (retain, getter=getChannelOverride,setter=setChannelOverride) NSMutableDictionary *channelOverride;
@property (assign, getter=getChannel,setter=setChannel) NSInteger channel;

+ (ANTLRCommonTokenStream *)newANTLRCommonTokenStream;
+ (ANTLRCommonTokenStream *)newANTLRCommonTokenStreamWithTokenSource:(id<ANTLRTokenSource>)theTokenSource;
+ (ANTLRCommonTokenStream *)newANTLRCommonTokenStreamWithTokenSource:(id<ANTLRTokenSource>)theTokenSource
                                                               Channel:(NSInteger)aChannel;

- (id) init;
- (id) initWithTokenSource:(id<ANTLRTokenSource>)theTokenSource;
- (id) initWithTokenSource:(id<ANTLRTokenSource>)theTokenSource Channel:(NSInteger)aChannel;

- (id<ANTLRTokenSource>) getTokenSource;
- (void) setTokenSource: (id<ANTLRTokenSource>) aTokenSource;

- (void) consume;
- (id<ANTLRToken>) LT:(NSInteger)k;
- (id<ANTLRToken>) LB:(NSInteger)k;

- (NSInteger) skipOffChannelTokens:(NSInteger) i;
- (NSInteger) skipOffChannelTokensReverse:(NSInteger) i;

- (void)setup;

- (NSArray *) tokensInRange:(NSRange)aRange;
- (NSArray *) tokensInRange:(NSRange)aRange inBitSet:(ANTLRBitSet *)aBitSet;
- (NSArray *) tokensInRange:(NSRange)aRange withTypes:(NSArray *)tokenTypes;
- (NSArray *) tokensInRange:(NSRange)aRange withType:(NSInteger)tokenType;

- (id<ANTLRToken>) getToken:(NSInteger)i;

- (NSInteger) size;
- (NSInteger) getIndex;
- (void) rewind;
- (void) rewind:(NSInteger)marker;
- (void) seek:(NSInteger)index;

- (NSString *) toString;
- (NSString *) toStringFromStart:(NSInteger)startIndex ToEnd:(NSInteger)stopIndex;
- (NSString *) toStringFromToken:(id<ANTLRToken>)startToken ToToken:(id<ANTLRToken>)stopToken;

- (id) copyWithZone:(NSZone *)aZone;

- (NSInteger)getChannel;
- (void)setChannel:(NSInteger)aChannel;

- (NSMutableDictionary *)getChannelOverride;
- (void)setChannelOverride:(NSMutableDictionary *)anOverride;

@end
