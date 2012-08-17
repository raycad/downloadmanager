/*!
 * \file contact.h
 * \brief
 *
 * Copyright of Nomovok Ltd. All rights reserved.
 *
 * Contact: nguyentruong.duong@nomovok.com
 *
 * \author Nguyen Truong Duong <nguyentruong.duong@nomovok.com>
 *
 */
#ifndef CONTACT_H
#define CONTACT_H

#include "common.h"

class Contact
{
public:
    enum ContactType {
        None = -1,
        NormalContact = 0,
        Url = 1,
        Application = 2,
        Music = 3
    };

    // Constructors
    Contact();
    Contact(int id, int linkId, ContactType contactType);

    /*!
     * \brief Set id of the contact
     * \param id: the id of the contact
     */
    void setId(int id);

    /*!
     * \brief Get id of the contact
     * \returns the id of the contact
     */
    int getId();

    /*!
     * \brief Set link id of the contact
     * \param linkId: the linkId of the contact
     */
    void setLinkId(int linkId);

    /*!
     * \brief Get linkId of the contact
     * \returns the linkId of the contact
     */
    int getLinkId();

    /*!
     * \brief Set contactType of the contact
     * \param contactType: the contactType of the contact
     */
    void setContactType(ContactType contactType);

    /*!
     * \brief Get contactType of the contact
     * \returns the contactType of the contact
     */
    ContactType getContactType();

private:
    int m_id;
    int m_linkId;
    ContactType m_contactType;
};

#endif // CONTACT_H
