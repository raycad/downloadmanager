/*!
 * \file contact.cpp
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#include "contact.h"

Contact::Contact()
{
    // Set default values
    m_id = -1;
    m_linkId = -1;
    m_contactType = None;
}

Contact::Contact(int id, int linkId, ContactType contactType)
{
    m_id = id;
    m_linkId = linkId;
    m_contactType = contactType;
}

void Contact::setId(int id)
{
    m_id = id;
}

int Contact::getId()
{
    return m_id;
}

void Contact::setLinkId(int linkId)
{
    m_linkId = linkId;
}

int Contact::getLinkId()
{
    return m_linkId;
}

void Contact::setContactType(ContactType contactType)
{
    m_contactType = contactType;
}

Contact::ContactType Contact::getContactType()
{
    return m_contactType;
}
