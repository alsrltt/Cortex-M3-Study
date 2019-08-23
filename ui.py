# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui.ui'
#
# Created by: PyQt5 UI code generator 5.11.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets
import serial

class Ui_MainWindow(object):
    ser = serial.Serial(
       port='COM25',
       baudrate=115200,
       parity=serial.PARITY_NONE,
       stopbits=serial.STOPBITS_ONE,
       bytesize=serial.EIGHTBITS,
       timeout=1)

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        self.LED1 = QtWidgets.QPushButton(self.centralwidget)
        self.LED1.setGeometry(QtCore.QRect(320, 80, 151, 61))
        self.LED1.setObjectName("LED1")

        self.LED2 = QtWidgets.QPushButton(self.centralwidget)
        self.LED2.setGeometry(QtCore.QRect(320, 240, 151, 61))
        self.LED2.setObjectName("LED2")

        self.LED3 = QtWidgets.QPushButton(self.centralwidget)
        self.LED3.setGeometry(QtCore.QRect(320, 410, 151, 61))
        self.LED3.setObjectName("LED3")

        MainWindow.setCentralWidget(self.centralwidget)

        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 26))
        self.menubar.setObjectName("menubar")

        MainWindow.setMenuBar(self.menubar)

        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")

        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.LED1.setText(_translate("MainWindow", "LED1"))
        self.LED2.setText(_translate("MainWindow", "LED2"))
        self.LED3.setText(_translate("MainWindow", "LED3"))

    def one_func(self,MainWindow):
        str='LED1'
        ser.write(serial.to_bytes([0x02]))
        ser.write(bytes(str.encode()))
        ser.write(serial.to_bytes([0x03]))

    def two_func(self,MainWindow):
        str='LED2'
        ser.write(serial.to_bytes([0x02]))
        ser.write(bytes(str.encode()))
        ser.write(serial.to_bytes([0x03]))

    def three_func(self,MainWindow):
        str='LED3'
        ser.write(serial.to_bytes([0x02]))
        ser.write(bytes(str.encode()))
        ser.write(serial.to_bytes([0x03]))

    def button_clicked(self,MainWindow):
        self.LED1.clicked.connect(self.one_func)
        self.LED2.clicked.connect(self.two_func)
        self.LED3.clicked.connect(self.three_func)


if __name__ == '__main__':
    import sys

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()

    ui = Ui_MainWindow()

    ui.setupUi(MainWindow)

    # ui.LED1_func(MainWindow)
    # ui.LED1_func(MainWindow)
    # ui.LED3_func(MainWindow)

    # ui.button_clicked(MainWindow)

    MainWindow.show()

    sys.exit(app.exec_())
