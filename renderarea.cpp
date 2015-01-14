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


#include <QtWidgets>
#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)	: QWidget(parent)
{
	//shape = Polygon;
	m_bAntialiased = false;
	m_bShowRefLines = false;
	//transformed = false;
	m_fXPos = 0.0f;
	m_fYPos = 0.0f;
	
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
}

RenderArea::~RenderArea()
{

}

//void RenderArea::setShape(Shape shape)
//{
	//this->shape = shape;
	//update();
//}

void RenderArea::setAntialiased(bool antialiased)
{
	this->m_bAntialiased = antialiased;
	update();
}

void RenderArea::UpdatePosition(float XPos, float YPos, bool bShowRef)
{//In Range 0 ... RESSCALEFACTOR
	m_fXPos = XPos;
	m_fYPos = YPos;
	m_bShowRefLines = bShowRef;
	update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, false);
	QPen pen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);

	QPoint CrossHair(((((float)(width()/RESSCALEFACTOR))*m_fXPos)+0.5f) ,((((float)(height()/RESSCALEFACTOR)))*m_fYPos)+0.5f);
	QRect rectCrossHair(CrossHair.x()-2,CrossHair.y()-2,4,4);
	QRect rectBox(0, 0, width()-1, height()-1);	
	painter.drawRect(rectBox);
	//painter.drawLine(rect.bottomLeft(), rect.topRight());
	//painter.drawText(rect, Qt::AlignCenter, tr("Qt by\nNokia"));
	//SQRRTTWO
	painter.drawRect(rectCrossHair);
	//painter.drawLine(CrossHair,CrossHair);
	//painter.drawLine(CrossHair,CrossHair);
	painter.setPen(palette().dark().color());
	if(m_bShowRefLines)
	{
		float XOffset = (1-INVSQRRTTWO)*(0.5*width());
		float YOffset = (1-INVSQRRTTWO)*(0.5*height());
		QRect rectRefLines(XOffset, YOffset, width()-(2*XOffset), height()-(2*YOffset));
		painter.drawRect(rectRefLines);
	}
	//rect.adjust(10,10,-10,-10);
	//painter.drawRect(rect);
	//painter.drawRect(QRect(0, 0, width() - 1, height() - 1));



	//static const QPoint points[4] = {
	//	QPoint(10, 80),
	//	QPoint(20, 10),
	//	QPoint(80, 30),
	//	QPoint(90, 70)
	//};
	//painter.setPen(pen);
	//painter.setBrush(brush);
	//if (antialiased)
	//	painter.setRenderHint(QPainter::Antialiasing, true);

	//for (int x = 0; x < width(); x += 100) {
	//	for (int y = 0; y < height(); y += 100) {
			//painter.save();
			//painter.translate(x, y);
	//		if (transformed) {
	//			painter.translate(50, 50);
	//			painter.rotate(60.0);
	//			painter.scale(0.6, 0.9);
	//			painter.translate(-50, -50);
	//		}
	//		switch (shape) {
	//		 case Line:
				 //painter.drawLine(rect.bottomLeft(), rect.topRight());
	//			 break;
	//		 case Points:
				 //painter.drawPoints(points, 4);
	//			 break;
	//		 case Polyline:
	//			 painter.drawPolyline(points, 4);
	//			 break;
	//		 case Polygon:
	//			 painter.drawPolygon(points, 4);
	//			 break;
	//		 case Rect:
	//			 painter.drawRect(rect);
	//			 break;
	//		 case RoundedRect:
	//			 painter.drawRoundedRect(rect, 25, 25, Qt::RelativeSize);
	//			 break;
	//		 case Ellipse:
	//			 painter.drawEllipse(rect);
	//			 break;
	//		 case Arc:
	//			 painter.drawArc(rect, startAngle, arcLength);
	//			 break;
	//		 case Chord:
	//			 painter.drawChord(rect, startAngle, arcLength);
	//			 break;
	//		 case Pie:
	//			 painter.drawPie(rect, startAngle, arcLength);
	//			 break;
	//		 case Path:
	//			 painter.drawPath(path);
	//			 break;
	//		 case Text:
	//			 painter.drawText(rect, Qt::AlignCenter, tr("Qt by\nNokia"));
	//			 break;
	//		 case Pixmap:
	//			 painter.drawPixmap(10, 10, pixmap);
	//		}
			//painter.restore();
		//}
	//}
}