/*
    This file is part of BlueFramework, a simple 3D engine.
	Copyright (c) 2016 Technical University of Munich
	Chair of Computational Modeling and Simulation.

    BlueFramework is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    BlueFramework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QWidget>
#include <QScreen>

void captureScreen() {
	QScreen *screen = QGuiApplication::primaryScreen();

	if (!screen)
		return;
	
	auto originalPixmap = screen->grabWindow(0);
	
	originalPixmap.save("C:/temp/screen.png");

}
