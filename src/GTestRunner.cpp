/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * GTestRunner.cpp - Created on 2010-07-23                                                   *
 *                                                                                           *
 * Copyright (C) 2010 Sandy Chapman                                                          *
 *                                                                                           *
 * This library is free software; you can redistribute it and/or modify it under the         *
 * terms of the GNU Lesser General Public License as published by the Free Software          *
 * Foundation; either version 2.1 of the License, or (at your option) any later version.     *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
 * See the GNU Lesser General Public License for more details.                               *
 * You should have received a copy of the GNU Lesser General Public License along with this  *
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, *
 * Boston, MA 02111-1307 USA                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "GTestRunner.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

GTestRunner::GTestRunner(QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags),
   fileMenu(tr("&File")), helpMenu(tr("&Help")), statusBar(this),
   centralWidget(this), testTreeTools(this), testTree(this),
   gtestList()
{
	resize(500, 800);
	setup();
}

GTestRunner::~GTestRunner()
{

}

void GTestRunner::setup() {
	//menus
	menuBar = QMainWindow::menuBar();
	setupMenus();
	setMenuBar(menuBar);

	//toolbar
	setupToolBars();

	setCentralWidget(&centralWidget);

	QObject::connect(&testTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
					 this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));

	setupLayout();
}

void GTestRunner::setupMenus() {
	QAction* newTestSetupAct = new QAction(tr("&New Test Setup..."), this);
	QAction* openTestSetupAct = new QAction(tr("&Open Test Setup..."), this);
	QAction* saveTestSetupAct = new QAction(tr("&Save Test Setup..."), this);
	QAction* importTestExeAct = new QAction(tr("&Import Test Executable"), this);
	QAction* quitAct = new QAction(tr("&Exit"), this);

	QObject::connect(importTestExeAct, SIGNAL(triggered()),
					 this, SLOT(addTests()));

	fileMenu.addAction(newTestSetupAct);
	fileMenu.addAction(openTestSetupAct);
	fileMenu.addAction(saveTestSetupAct);
	fileMenu.addAction(importTestExeAct);
	fileMenu.addAction(quitAct);

	QAction* aboutAct = new QAction(tr("&About GTestRunner..."), this);
	QAction* aboutQtAct = new QAction(tr("About &Qt..."), this);

	helpMenu.addAction(aboutAct);
	helpMenu.addAction(aboutQtAct);

	menuBar->addMenu(&fileMenu);
	menuBar->addMenu(&helpMenu);
}

void GTestRunner::setupToolBars() {
	QAction* runTestsAct = new QAction(tr("Run"), this);
	QAction* stopTestsAct = new QAction(tr("Stop"), this);
	QAction* addTestsAct = new QAction(tr("Add"), this);
	QAction* removeTestsAct = new QAction(tr("Remove"), this);

	QObject::connect(addTestsAct, SIGNAL(triggered()),
					 this, SLOT(addTests()));

	testTreeTools.addAction(runTestsAct);
	testTreeTools.addAction(stopTestsAct);
	testTreeTools.addAction(addTestsAct);
	testTreeTools.addAction(removeTestsAct);
}

void GTestRunner::setupLayout() {
	QGroupBox *treeBox = new QGroupBox(tr("Unit Tests Loaded"));
	QVBoxLayout *treeLayout = new QVBoxLayout;
	treeLayout->addWidget(&testTreeTools);
	treeLayout->addWidget(&testTree);
	treeBox->setLayout(treeLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(treeBox);

	centralWidget.setLayout(mainLayout);
}

void GTestRunner::addTests() {
	bool addResolved = false;
	QString filepath;
	GTest* newTest = new GTest();
	while(!addResolved) {
		filepath = QFileDialog::getOpenFileName(this, tr("Select Google Test Executable"));
		qDebug() << "File path received:" << filepath;
		if(filepath.isEmpty())
			return;

		newTest->setExecutablePath(filepath);
		GTest::STATE state = newTest->getState();
		switch(state) {
			case GTest::FILE_NOT_FOUND: {
				QMessageBox::StandardButton btnPressed = QMessageBox::warning(this,
						tr("File Not Found"),
						tr("It appears the filename entered does not exist. Please select a valid Google test executable."),
						QMessageBox::Retry | QMessageBox::Abort);
				switch(btnPressed) {
					case QMessageBox::Retry:
						continue; //go back to top of loop.
					case QMessageBox::Abort: default:
						return;   //exit attempt to add a test executable
					}
			break;
			}
			case GTest::INSUFFICIENT_PRIVILEGES: {
				QMessageBox::StandardButton btnPressed = QMessageBox::warning(this,
						tr("Insufficient Permissions"),
						tr("It appears that you do not have sufficient privileges to execute this file. \
						   GTestRunner can attempt to reset the permissions to enable executing this test file. \
						   \nWould you like GTestRunner to attempt this?"),
						QMessageBox::Yes | QMessageBox::No);
				switch(btnPressed) {
					case QMessageBox::Yes:
						continue;
					case QMessageBox::No: default: {
						QMessageBox::StandardButton btnPressed = QMessageBox::question(this,
							tr("Try Again?"),
							tr("Would you like to try again and select another file?"),
							QMessageBox::Yes | QMessageBox::No);
						switch(btnPressed) {
							case QMessageBox::Yes:
								continue;
							case QMessageBox::No: default:
								return;
						}
					break;
					}
				}
				break;
			}
			case GTest::VALID: {
				addResolved = true;
				break;
			}
		}
	}

	invokeListingRetrieval(filepath);
}

void GTestRunner::invokeListingRetrieval(QString filepath) {
	GTest *listing = new GTest(filepath);
	QObject::connect(listing, SIGNAL(listingReady(GTest*)),
					 this, SLOT(updateListing(GTest*)));
	listing->produceListing();
}

void GTestRunner::updateListing(GTest* listing) {
	qDebug() << "Updating listing";
	const int exitCode = listing->getExitCode();
	qDebug() << "got exit code:" << exitCode;
	QString exePath = listing->getExecutablePath();
	qDebug() << "got exe path:" << exePath;
	if(exitCode != 0) {
		QMessageBox::critical(this, "Error Retrieving Test Listing",
					QString("Exit code").append(exitCode).append("was returned from the Google Test executable at").append(exePath).append(". Are you sure this is a valid Google Test unit test executable?"));
		//TODO: perform switch statement of process error.
		return;
	}
	QStringList testList = listing->getListing();
	qDebug() << "Retrieved listing. Size ="<<testList.size();
	QStringList::iterator it = testList.begin();

	QTreeWidgetItem* testContainer = new QTreeWidgetItem(&testTree,QStringList() << exePath);
	testContainer->setFlags(Qt::ItemIsTristate | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	testContainer->setCheckState(0, Qt::Checked);

	QTreeWidgetItem* topLevelItem = 0;
	QTreeWidgetItem* newItem = 0;
	while(it != testList.end()) {
		qDebug() << *it;
		if(it->endsWith(".")) {
			//drop the '.' and make it a data item
			topLevelItem = new QTreeWidgetItem(testContainer, QStringList()<<(it->left(it->size()-1)));
			topLevelItem->setFlags(Qt::ItemIsTristate | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			topLevelItem->setCheckState(0, Qt::Checked);
		}
		else {
			//drop the spaces and make it a data item
			newItem = new QTreeWidgetItem(topLevelItem, QStringList()<<(it->right(it->size()-2)));
			newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			newItem->setCheckState(0,Qt::Checked);
		}
		++it;
	}
}

void GTestRunner::updateAllListings() {

}

void GTestRunner::runTests() {

}

/*
 *	This function handles correcting the check states. If a parent is
 *	part checked and all its children are part checked, we transition
 *	to fully checked (previous state must have been unchecked and a click
 *	must have been made on the parent). If any of the children are not
 *	part checked, we know that we did not in fact click on the parent
 *	(and thus we can immediately return), so a part checked parent is fine.
 */
void GTestRunner::treeItemClicked(QTreeWidgetItem* item, int column) {
	if(item->childCount() == 0 || item->parent() == 0)
		return;
	if(item->checkState(0) ==Qt::PartiallyChecked) {
		for(int i=0,j=item->childCount();i<j;i++)
			if(item->child(i)->checkState(0) & (Qt::Unchecked | Qt::Checked))
				return;
		item->setCheckState(0,Qt::Checked);
	}
}

