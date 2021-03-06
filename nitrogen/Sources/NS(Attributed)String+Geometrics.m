/*=========================================================================
  Program:   OsiriX

  Copyright (c) OsiriX Team
  All rights reserved.
  Distributed under GNU - LGPL
  
  See http://www.osirix-viewer.com/copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.
 ---------------------------------------------------------------------------
 
 This file is part of the Horos Project.
 
 Current contributors to the project include Alex Bettarini and Danny Weissman.
 
 Horos is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation,  version 3 of the License.
 
 Horos is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Horos.  If not, see <http://www.gnu.org/licenses/>.

 

 
 ---------------------------------------------------------------------------
 
 This file is part of the Horos Project.
 
 Current contributors to the project include Alex Bettarini and Danny Weissman.
 
 Horos is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation,  version 3 of the License.
 
 Horos is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Horos.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/

#import "NS(Attributed)String+Geometrics.h"

int gNSStringGeometricsTypesetterBehavior = NSTypesetterLatestBehavior ;

@implementation NSAttributedString (Geometrics) 

#pragma mark Measure Attributed String

- (NSSize)sizeForWidth:(float)width 
				height:(float)height {
	NSSize answer = NSZeroSize ;
    if ([self length] > 0) {
		// Checking for empty string is necessary since Layout Manager will give the nominal
		// height of one line if length is 0.  Our API specifies 0.0 for an empty string.
		NSSize size = NSMakeSize(width, height) ;
		NSTextContainer *textContainer = [[NSTextContainer alloc] initWithContainerSize:size] ;
		NSTextStorage *textStorage = [[NSTextStorage alloc] initWithAttributedString:self] ;
		NSLayoutManager *layoutManager = [[NSLayoutManager alloc] init] ;
		[layoutManager addTextContainer:textContainer] ;
		[textStorage addLayoutManager:layoutManager] ;
		[layoutManager setHyphenationFactor:0.0] ;
		if (gNSStringGeometricsTypesetterBehavior != NSTypesetterLatestBehavior) {
			[layoutManager setTypesetterBehavior:gNSStringGeometricsTypesetterBehavior] ;
		}
		// NSLayoutManager is lazy, so we need the following kludge to force layout:
		[layoutManager glyphRangeForTextContainer:textContainer] ;
		
		answer = [layoutManager usedRectForTextContainer:textContainer].size ;
		[textStorage release] ;
		[textContainer release] ;
		[layoutManager release] ;
		
		// In case we changed it above, set typesetterBehavior back
		// to the default value.
		gNSStringGeometricsTypesetterBehavior = NSTypesetterLatestBehavior ;
	}
	
	return answer ;
}

- (float)heightForWidth:(float)width {
	return [self sizeForWidth:width
					   height:FLT_MAX].height ;
}

- (float)widthForHeight:(float)height {
	return [self sizeForWidth:FLT_MAX
					   height:height].width ;
}

@end


@implementation NSString (Geometrics)

#pragma mark Given String with Attributes

- (NSSize)sizeForWidth:(float)width 
				height:(float)height
			attributes:(NSDictionary*)attributes {
	NSSize answer ;
	
	NSAttributedString *astr = [[NSAttributedString alloc] initWithString:self
															   attributes:attributes] ;
	answer = [astr sizeForWidth:width
									height:height] ;
	[astr release] ;
	
	return answer ;
}

- (float)heightForWidth:(float)width
			 attributes:(NSDictionary*)attributes {
	return [self sizeForWidth:width
					   height:FLT_MAX
				   attributes:attributes].height ;
}

- (float)widthForHeight:(float)height
			 attributes:(NSDictionary*)attributes {
	return [self sizeForWidth:FLT_MAX
					   height:height
				   attributes:attributes].width ;
}

#pragma mark Given String with Font

- (NSSize)sizeForWidth:(float)width 
				height:(float)height
				  font:(NSFont*)font {
	NSSize answer = NSZeroSize ;
	
	if (font == nil) {
		NSLog(@"[%@]: Error: cannot compute size with nil font", [self class]) ;
	}
	else {
		NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:
									font, NSFontAttributeName, nil] ;
		answer = [self sizeForWidth:width
							 height:height
						 attributes:attributes] ;
	}
	return answer ;
}

- (float)heightForWidth:(float)width
				   font:(NSFont*)font {
	return [self sizeForWidth:width
					   height:FLT_MAX
						 font:font].height ;
}

- (float)widthForHeight:(float)height
				   font:(NSFont*)font {
	return [self sizeForWidth:FLT_MAX
					   height:height
						 font:font].width ;
}

@end
