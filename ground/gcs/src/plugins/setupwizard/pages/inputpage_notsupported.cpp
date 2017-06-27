/**
 ******************************************************************************
 *
 * @file       inputpage.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @see        The GNU Public License (GPL) Version 3
 *
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup SetupWizard Setup Wizard
 * @{
 * @brief
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 */

#include "inputpage_notsupported.h"
#include "ui_inputpage_notsupported.h"
#include "setupwizard.h"

InputPageNotSupported::InputPageNotSupported(SetupWizard *wizard, QWidget *parent)
    : AbstractWizardPage(wizard, parent)
    ,

    ui(new Ui::InputPageNotSupported)
{
    ui->setupUi(this);
}

InputPageNotSupported::~InputPageNotSupported()
{
    delete ui;
}

bool InputPageNotSupported::validatePage()
{
    return true;
}
