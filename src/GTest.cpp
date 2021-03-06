/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTest.cpp - Created on 2010-07-25
 *
 * Copyright (C) 2010 Sandy Chapman
 *
 * This library is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "GTest.h"
#include "GTestResults.h"
#include "GTestSuite.h"

/*! \brief Constructor
 *
 * \param parent A pointer to the parent object. This should be a GTestSuite.
 * \param name The name of the unit test. If called directly, this should be
 * 			   the value \em testName in \code GTEST(testCase, testName) \endcode
 */
GTest::GTest(QObject* parent, QString name)
: QObject(parent), testResults(0)
{
	setObjectName(name);
}

/*! \brief Constructor
 *
 * This is the same as the constructor with the QObject* argument,
 * except that addTest is called on the parent.
 * \param parent A pointer to the parent test.
 * \param name The name of the unit test. If called directly, this should be
 * 			   the value \em testName in \code GTEST(testCase, testName) \endcode
 */
GTest::GTest(GTestSuite* parent, QString name)
: QObject(parent), testResults(0)
{
	setObjectName(name);
	if(parent)
		parent->addTest(this);
}

/*! \bried Destructor
 *
 * Deletes the testResult object if it exists.
 */
GTest::~GTest() {
	if(testResults)
		delete testResults;
}

/*! \brief Receives a test result object from a GTestSuite object.
 *
 * \param testResults The test result object, or 0 if there was
 * 					  an error in processing.
 */
void GTest::receiveTestResults(GTestResults* testResults) {
	if(this->testResults != 0)
		delete this->testResults;
	this->testResults = testResults;
	emit testResultsReady();
}



