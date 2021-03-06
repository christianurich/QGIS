/***************************************************************************
    qgswebviewwidget.cpp
     --------------------------------------
    Date                 : 5.1.2014
    Copyright            : (C) 2014 Matthias Kuhn
    Email                : matthias dot kuhn at gmx dot ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgswebviewwidget.h"

#include "qgsfilterlineedit.h"
#include "qgsnetworkaccessmanager.h"

#include <QGridLayout>
#include <QFileDialog>

QgsWebViewWidget::QgsWebViewWidget( QgsVectorLayer* vl, int fieldIdx, QWidget* editor, QWidget* parent )
    :  QgsEditorWidgetWrapper( vl, fieldIdx, editor, parent )
{
}

void QgsWebViewWidget::loadUrl( const QString &url )
{
  if ( mWebView )
    mWebView->load( url );
}

QVariant QgsWebViewWidget::value()
{
  QVariant v;

  if ( mLineEdit )
    v = mLineEdit->text();

  return v;
}

QWidget* QgsWebViewWidget::createWidget( QWidget* parent )
{
  QWidget* container = new QWidget( parent );
  QGridLayout* layout = new QGridLayout( container );
  QgsFilterLineEdit* le = new QgsFilterLineEdit( container );
  QWebView* webView = new QWebView( parent );
  webView->setObjectName( "EditorWebView" );
  QPushButton* pb = new QPushButton( tr( "..." ), container );
  pb->setObjectName( "FileChooserButton" );

  layout->addWidget( webView, 0, 0, 1, 2 );
  layout->addWidget( le, 1, 0 );
  layout->addWidget( pb, 1, 1 );

  container->setLayout( layout );

  return container;
}

void QgsWebViewWidget::initWidget( QWidget* editor )
{
  QWidget* container;

  mLineEdit = qobject_cast<QLineEdit*>( editor );

  if ( mLineEdit )
  {
    container = qobject_cast<QWidget*>( mLineEdit->parent() );
  }
  else
  {
    container = editor;
    mLineEdit = container->findChild<QLineEdit*>();
  }

  mButton = container->findChild<QPushButton*>( "FileChooserButton" );
  if ( !mButton )
    mButton = container->findChild<QPushButton*>();

  mWebView = container->findChild<QWebView*>( "EditorWebView" );
  if ( !mWebView )
    mWebView = container->findChild<QWebView*>();

  if ( mWebView )
  {
    mWebView->page()->setNetworkAccessManager( QgsNetworkAccessManager::instance() );
    mWebView->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
    mWebView->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
#ifdef QGISDEBUG
    mWebView->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
#endif
  }

  if ( mButton )
    connect( mButton, SIGNAL( clicked() ), this, SLOT( selectFileName() ) );

  if ( mLineEdit )
  {
    connect( mLineEdit, SIGNAL( textChanged( QString ) ), this, SLOT( loadUrl( QString ) ) );
    connect( mLineEdit, SIGNAL( textChanged( QString ) ), this, SLOT( valueChanged( QString ) ) );
  }
}

void QgsWebViewWidget::setValue( const QVariant& value )
{
  if ( mLineEdit )
    mLineEdit->setText( value.toString() );

  loadUrl( value.toString() );
}

void QgsWebViewWidget::setEnabled( bool enabled )
{
  if ( mLineEdit )
    mLineEdit->setEnabled( enabled );

  if ( mButton )
    mButton->setEnabled( enabled );
}

void QgsWebViewWidget::selectFileName()
{
  QString text;

  if ( mLineEdit )
    text = mLineEdit->text();

  QString fileName = QFileDialog::getOpenFileName( mLineEdit, tr( "Select a file" ), QFileInfo( text ).absolutePath() );

  if ( fileName.isNull() )
    return;

  if ( mLineEdit )
    mLineEdit->setText( QDir::toNativeSeparators( fileName ) );
}
