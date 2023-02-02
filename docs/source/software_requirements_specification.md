# Software Requirements Specification
## For libdomain

Wed Feb 1 2023
Version 0.1.0
BaseALT Ltd
Prepared by august@basealt.ru

Table of Contents
=================
* [Revision History](#revision-history)
* 1 [Introduction](#1-introduction)
  * 1.1 [Document Purpose](#11-document-purpose)
  * 1.2 [Product Scope](#12-product-scope)
  * 1.3 [Definitions, Acronyms and Abbreviations](#13-definitions-acronyms-and-abbreviations)
  * 1.4 [References](#14-references)
  * 1.5 [Document Overview](#15-document-overview)
* 2 [Product Overview](#2-product-overview)
  * 2.1 [Product Perspective](#21-product-perspective)
  * 2.2 [Product Functions](#22-product-functions)
  * 2.3 [User Classes and Characteristics](#23-user-classes-and-characteristics)
  * 2.4 [Operating Environment](#24-operating-environment)
  * 2.5 [Design and Implementation Constraints](#25-design-and-implementation-constraints)
  * 2.6 [User Documentation](#26-user-documentation)
  * 2.7 [Assumptions and Dependencies](#27-assumptions-and-dependencies)
* 3 [Requirements](#3-requirements)
  * 3.1 [External interface requirements](#31-external-interface-requirements)
    * 3.1.1 [Hardware interface requirements](#311-hardware-interface-requirements)
    * 3.1.2 [Software interface requirements](#312-software-interface-requirements)
    * 3.1.3 [Communication interface requirements](#313-communication-interface-requirements)
  * 3.2 [Functional Requirements](#32-functional-requirements)
  * 3.3 [Quality of Service](#33-quality-of-service)
* 4 [Verification](#4-verification)
* 5 [Appendixes](#5-appendixes)

## Revision History
| Version   | Name  | Date           | Brief Description   |
| --------- | ------| -------------- | ------------------- |
|   0.1.0   | Draft | Wed Feb 1 2023 | First draft.        |
|           |       |                |                     |
|           |       |                |                     |

## 1. Introduction
This specification documents the system level requirements for the libdomain project. The objectives of this specification are to provide a system overview including definition, goals, objectives, context, and major capabilities.

This specification is organized into following sections:
* [Introduction](#1-introduction), which introduces the specification for libdomain to its readers.
* [Product Overview](#2-product-overview), which provides a brief, high level description of the libdomain including its definition, and its goals.
* [Requirements](#3-requirements), which holds list of functional requirements and key qualify indicators.
* [Verification](#4-verification), which provides the verification approaches and methods planned to qualify the software.

### 1.1 Document Purpose

### 1.2 Product Scope

### 1.3 Definitions, Acronyms and Abbreviations
| Acronym or Abbreviation | Full description                      |
| ----------------------- | ------------------------------------- |
| SRS                     | Software specification requirements   |
| LDAP                    | Lightweight directory access protocol |
| LDIF                    | The LDAP Data Interchange Format      |

### 1.4 References

List of core RFCs for LDAP:
* [RFC4510](https://www.rfc-editor.org/rfc/rfc4510.txt): Technical Specification Road Map
* [RFC4511](https://www.rfc-editor.org/rfc/rfc4511.txt): The Protocol
* [RFC4512](https://www.rfc-editor.org/rfc/rfc4512.txt): Directory Information Models
* [RFC4513](https://www.rfc-editor.org/rfc/rfc4513.txt): Authentication Methods and Security Mechanisms
* [RFC4514](https://www.rfc-editor.org/rfc/rfc4514.txt): String Representation of Distinguished Names
* [RFC4515](https://www.rfc-editor.org/rfc/rfc4515.txt): String Representation of Search Filters
* [RFC4516](https://www.rfc-editor.org/rfc/rfc4516.txt): Uniform Resource Locator
* [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt): Syntaxes and Matching Rules
* [RFC4518](https://www.rfc-editor.org/rfc/rfc4518.txt): Internationalized String Preparation
* [RFC4519](https://www.rfc-editor.org/rfc/rfc4519.txt): Schema for User Applications

List of additional RFCs for LDAP:
* [RFC2696](https://www.rfc-editor.org/rfc/rfc2696.txt): LDAP Control Extension for Simple Paged Results Manipulation
* [RFC2849](https://www.rfc-editor.org/rfc/rfc2849.txt): The LDAP Data Interchange Format (LDIF) - Technical Specification
* [RFC3045](https://www.rfc-editor.org/rfc/rfc3045.txt): Storing Vendor Information in the LDAP root DSE
* [RFC3062](https://www.rfc-editor.org/rfc/rfc3062.txt): LDAP Password Modify Extended Operation
* [RFC4525](https://www.rfc-editor.org/rfc/rfc4525.txt): Modify-Increment Extension
* [RFC4530](https://www.rfc-editor.org/rfc/rfc4530.txt): entryUUID Operational Attribute
* [RFC4532](https://www.rfc-editor.org/rfc/rfc4532.txt): "Who am I?" Operation
* [RFC5020](https://www.rfc-editor.org/rfc/rfc5020.txt): entryDN Operational Attribute

## 2. Product Overview

### 2.1 Product Perspective
### 2.2 Product Functions
Main features, which are offered by the library:
* Manage LDAP entries: add, update, replace, delete.
* Loading object and class schemas from LDAP service or from local source.
* Searching objects by submitting search queries.
* Creating or modifying objects by providing changes in LDIF format.
* Multiple connection strategies: sync, async, mock sync, mock async.
* Additional abstraction layer to create LDAP objects and attributes and perform their verification.

### 2.3 User Classes and Characteristics
Target users are software developers.

### 2.4 Operating Environment

### 2.5 Design and Implementation Constraints
List of design and implementation constraints:
* Library must be implmented using C programming language.
* Library must utilize CMake as build system.

### 2.6 User Documentation
Documentation for end users will be made available from project site. README.md file will contain link to the documentation page.
Set of documentation will be developed alongside with source code in restructuredtext and markdown formats.

### 2.7 Assumptions and Dependencies

## 3. Requirements

### 3.1 External Interface Requirements
This section and subsequent subsections provide detailed description of all inputs into and outputs from the software. 
This section and subsections should include both content and format as follows:
* Name of item
* Description of purpose
* Source of input or destination of output
* Valid range, accuracy, and/or tolerance
* Units of measure
* Timing
* Relationships to other inputs/outputs
* Data formats
* Command formats
* End messages
These requirements may be organized in the following subsections.

#### 3.1.1 Hardware Interface Requirements
#### 3.1.2 Software Interface Requirements
#### 3.1.3 Communication Interface Requirements

## 3.2 Functional Requirements
## 3.3 Quality of Service

## 4. Verification

## 5. Appendixes
