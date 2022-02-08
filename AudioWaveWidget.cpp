#include "AudioWaveWidget.h"

void AudioWaveWidget::initializeGL(void)
{
	nDebug("AudioWaveWidget::initializeGL");
}

void AudioWaveWidget::resizeGL(int w, int h)
{
}

void AudioWaveWidget::paintGL(void)
{
}

AudioWaveWidget::AudioWaveWidget()
{
}

AudioWaveWidget::AudioWaveWidget(QWidget* parent)
{
	m_parent = parent;
}

AudioWaveWidget::~AudioWaveWidget()
{
}
