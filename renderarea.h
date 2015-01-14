//USBHIDDeviceplugin
//Copyright (C) 2015  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QWidget>
#include "global.h"
//#include <QPen>
//#include <QPixmap>

class RenderArea : public QWidget
{
	Q_OBJECT

public:
	RenderArea(QWidget *parent = NULL);
	~RenderArea();

public slots:
	void UpdatePosition(float XPos, float YPos, bool bShowRef = false);
	void setAntialiased(bool antialiased);

protected:
	void paintEvent(QPaintEvent *event);

private:
	//Shape shape;
	//QPen pen;
	//QBrush brush;
	bool m_bAntialiased;
	bool m_bShowRefLines;
	float m_fXPos;
	float m_fYPos;
};

#endif // RENDERAREA_H
