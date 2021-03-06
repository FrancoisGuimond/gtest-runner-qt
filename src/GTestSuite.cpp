/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestSuite.cpp - Created on 2010-07-25
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

#include "GTestSuite.h"
#include "GTestResults.h"

/*! \brief Constructor
 *
 * \param parent A pointer to the parent object. This should be a GTestExecutable.
 * \param name The name of the unit test suite. If called directly, this should
 * 		  be the value \em testCase in \code GTEST(testCase, testName) \endcode
 */
GTestSuite::GTestSuite(QObject* parent, QString name)
: GTest(parent, name), runList()
{}

/*! \brief Constructor
 *
 * This is the same as the constructor with the QObject* argument,
 * except that addTest is called on the parent.
 * \param parent A pointer to the parent test.
 * \param name The name of the unit test. If called directly, this should be
 * 			   the value \em testName in \code GTEST(testCase, testName) \endcode
 */
GTestSuite::GTestSuite(GTestSuite* parent, QString name)
: GTest(parent, name), runList()
{}

/*! \brief  Destructor
 */
GTestSuite::~GTestSuite()
{}

/*! \brief A slot that receives a run request from a child GTest.
 *
 * Receives a run request from a child test. Adds the test to its runlist.
 * If received by a GTestSuite, this will emit a request with its name as
 * the testCase.
 * \todo TODO::Store testName and testCase for selective execution by the GTestExecutable
 * \param testName The name of the GTest to run.
 * \param testCase The name of the GTestSuite to run. Note that
 * 				   this is 'null' if sent from a GTest, and is the
 * 				   name of the GTestSuite if received in a GTestExecutable.
 */
void GTestSuite::receiveRunRequest(QString testName, QString testCase) {
	GTest* test = static_cast<GTest*>(QObject::sender());
	if(!runList.contains(test))
		runList.append(test);
	if(testCase.isEmpty()) {
		QString objectName = this->objectName();
		emit requestingRun(testName, objectName);
	}
}

/*! \brief Receives the test results from the parent GTestExecutable object.
 *
 * This function receives the GTestSuiteResult from the parent object and
 * iterates through its list of tests to run, and gives them there appropriate
 * test results. The GTestResults object holds the information in a hash
 * table so retrieve should be approximately O(1).
 */
void GTestSuite::receiveTestResults(GTestResults* testSuiteResults) {
	QList<GTest*>::iterator it = runList.begin();
	GTestResults* testResults;
	while(it != runList.end()) {
        QString testName = (*it)->objectName();
        testResults = testSuiteResults->getTestResults(testName);
        if (testResults){
            (*it)->receiveTestResults(testResults);
        }
		++it;
	}
	runList.clear();
	GTest::receiveTestResults(testSuiteResults);
	emit testResultsReady();
}

/*! \brief Sets all tests in the suite to be run.
 *
 * This test should be run by right-clicking and selecting run on
 * a test suite object.
 */
void GTestSuite::run() {
	runList.clear();
	runList.append(reinterpret_cast<const QList<GTest*>& >(this->children()));
	QList<GTest*>::iterator it = runList.begin();
	while(it != runList.end()) {
		(*it)->run();
		++it;
	}
}

