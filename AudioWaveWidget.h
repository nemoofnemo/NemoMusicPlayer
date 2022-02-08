#pragma once

#include <memory>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "libnemo/nemo_utilities.h"

class AudioWaveWidget :
	public QOpenGLWidget,
	protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

private:
	//(x1, y1), (x2, y2), (x3, y3) ...
	//length of wave_data is sample_count * 2.
	float* wave_data = nullptr;
	int sample_count = 0;
	QWidget* m_parent = nullptr;

protected:
	virtual void initializeGL(void) override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL(void) override;
	void update_preview(void);
	void update_data(const float* const ptr);
	void clear_preview(void);

public:
	AudioWaveWidget();
	explicit AudioWaveWidget(QWidget* parent);
	virtual~AudioWaveWidget();
};

