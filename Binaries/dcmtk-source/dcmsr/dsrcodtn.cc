/*
 *
 *  Copyright (C) 2000-2005, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRCodeTreeNode
 *
 *  Last Update:      $Author: lpysher $
 *  Update Date:      $Date: 2006/03/01 20:16:11 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrcodtn.h"
#include "dsrxmld.h"


DSRCodeTreeNode::DSRCodeTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Code),
   DSRCodedEntryValue()
{
}


DSRCodeTreeNode::~DSRCodeTreeNode()
{
}


void DSRCodeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRCodedEntryValue::clear();
}


OFBool DSRCodeTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRCodedEntryValue::isValid() && getConceptName().isValid();
}


OFCondition DSRCodeTreeNode::print(ostream &stream,
                                   const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        DSRCodedEntryValue::print(stream, OFTrue /*printCodeValue*/, OFTrue /*printInvalid*/);
    }
    return result;
}


OFCondition DSRCodeTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* goto "value" element */
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "value");
        if (childCursor.valid())
        {
            /* check whether code is stored as XML elements or attributes */
            if (doc.hasAttribute(childCursor, "codValue"))
                result = DSRCodedEntryValue::readXML(doc, childCursor);
            else
                result = DSRCodedEntryValue::readXML(doc, cursor);
        }
    }
    return result;
}


OFCondition DSRCodeTreeNode::writeXML(ostream &stream,
                                      const size_t flags,
                                      OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    if (flags & DSRTypes::XF_codeComponentsAsAttribute)
    {
        stream << "<value";     // bracket ">" is closed in next the writeXML() routine
        DSRCodedEntryValue::writeXML(stream, flags, logStream);
        stream << "</value>" << endl;
    } else
        DSRCodedEntryValue::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRCodeTreeNode::readContentItem(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read ConceptCodeSequence */
    return DSRCodedEntryValue::readSequence(dataset, DCM_ConceptCodeSequence, "1" /*type*/, logStream);
}


OFCondition DSRCodeTreeNode::writeContentItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write ConceptCodeSequence */
    return DSRCodedEntryValue::writeSequence(dataset, DCM_ConceptCodeSequence, logStream);
}


OFCondition DSRCodeTreeNode::renderHTMLContentItem(ostream &docStream,
                                                   ostream & /*annexStream*/,
                                                   const size_t /*nestingLevel*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render Code */
    if (result.good())
    {
        const OFBool fullCode = (flags & DSRTypes::HF_renderInlineCodes) || (flags & DSRTypes::HF_renderItemsSeparately);
        if (!fullCode)
            docStream << "<u>";
        result = DSRCodedEntryValue::renderHTML(docStream, flags, logStream, fullCode);
        if (!fullCode)
            docStream << "</u>";
        docStream << endl;
    }
    return result;
}


OFBool DSRCodeTreeNode::containsExtendedCharacters() const
{
  return DSRDocumentTreeNode::containsExtendedCharacters() || valueContainsExtendedCharacters();
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcodtn.cc,v $
 *  Revision 1.1  2006/03/01 20:16:11  lpysher
 *  Added dcmtkt ocvs not in xcode  and fixed bug with multiple monitors
 *
 *  Revision 1.20  2005/12/08 15:47:40  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.19  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.18  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.17  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.16  2003/08/07 13:11:53  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.15  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.14  2001/11/19 13:16:58  joergr
 *  Adapted implementation according to correction proposal 266.
 *
 *  Revision 1.13  2001/11/09 16:12:45  joergr
 *  Added new command line option allowing to encode codes as XML attributes
 *  (instead of tags).
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.12  2001/10/10 15:29:48  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.11  2001/09/26 13:04:17  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.10  2001/05/07 16:14:22  joergr
 *  Updated CVS header.
 *
 *  Revision 1.9  2001/02/02 14:41:55  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.8  2000/11/09 20:33:59  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.7  2000/11/07 18:33:28  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/11/01 16:28:47  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.5  2000/10/26 14:26:54  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:45  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:12:53  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:01:55  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:16  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
