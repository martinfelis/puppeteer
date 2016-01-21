/*
 * MeshUp - A visualization tool for multi-body systems based on skeletal
 * animation and magic.
 *
 * Copyright (c) 2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef RENDERIMAGEDIALOG_H
#define RENDERIMAGEDIALOG_H
 
#include "ui_PuppeteerAboutDialog.h"

class PuppeteerAboutDialog : public QDialog, public Ui::PuppeteerAboutDialog {
    Q_OBJECT
 
public:
		PuppeteerAboutDialog (QWidget *parent = 0) {
			setupUi(this);
		}

};

#endif
