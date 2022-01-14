#pragma once

#include <memory>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class AudioWaveWidget :
	public QOpenGLWidget,
	protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

private:
	std::shared_ptr<float> wave_data = nullptr;
	int sample_count = 0;

protected:
	void initializeGL(void) override;
	void resizeGL(int w, int h) override;
	void paintGL(void) override;
	void update_preview(void);
	void update_data(const float* const ptr);
	void clear_preview(void);
};

