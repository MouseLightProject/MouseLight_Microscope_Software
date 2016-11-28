#include "MySliderWithMultipleHandles.h"

//https://forum.qt.io/topic/73019/slider-with-two-handles-styles-support/2
MySliderWithMultipleHandles::MySliderWithMultipleHandles(channelHistogramInformationStruct * channelHistogramInformationInput, size_t *currentIndexInput, QWidget * parent)
	: QSlider(Qt::Horizontal, parent),
	  currentlySelected(-1),
	  minDistanceBetweenSliders(1000),
	  channelHistogramInformation(channelHistogramInformationInput),
	  currentIndex(currentIndexInput)
{// installEventFilter(this);
}

void MySliderWithMultipleHandles::paintEvent(QPaintEvent *ev)
{
	Q_UNUSED(ev);
	QPainter p(this);
	QStyleOptionSlider opt;
	setMaximum(65535);
	setMinimum(0);
	setSingleStep(1);
	setPageStep(0);
	//Groove
	initStyleOption(&opt);
	opt.subControls = QStyle::SC_SliderGroove;
	opt.rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
	style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

	//First handle.
	initStyleOption(&opt);
					opt.sliderPosition = channelHistogramInformation[*currentIndex].minValue;
	opt.subControls = QStyle::SC_SliderHandle;
	opt.rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
	sliderWidthInSliderCoordinates = ((maximum() - minimum()) * 1.0*opt.rect.width()) / width();
	style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);
		//Second handle
	initStyleOption(&opt);
		opt.sliderPosition = channelHistogramInformation[*currentIndex].maxValue;
	opt.subControls = QStyle::SC_SliderHandle;
	opt.rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
	style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);
	if (currentIndexPrevious!=*currentIndex)
	{	
		emit minimumValueChanged(QString("Minimum: %1").arg(channelHistogramInformation[*currentIndex].minValue,6));
		emit maximumValueChanged(QString("Maximum: %1").arg(channelHistogramInformation[*currentIndex].maxValue,6));
		currentIndexPrevious = *currentIndex;
	}
}



void MySliderWithMultipleHandles::mouseMoveEvent(QMouseEvent *ev)
{	mousePositionInSliderCoordinates = minimum() + ((maximum() - minimum()) * ev->x() )/ width();
	minValue = channelHistogramInformation[*currentIndex].minValue;
	maxValue = channelHistogramInformation[*currentIndex].maxValue;
	if ( minValue - sliderWidthInSliderCoordinates*minValue/maximum() < mousePositionInSliderCoordinatesForSliderSelection
		&& minValue + sliderWidthInSliderCoordinates*(1-minValue/maximum()) > mousePositionInSliderCoordinatesForSliderSelection
		&& currentlySelected == -1
		&& justPushed)
	{
		setSliderPosition(minValue);
		currentlySelected = 0;
	}
	else if ( maxValue - sliderWidthInSliderCoordinates*maxValue/maximum() < mousePositionInSliderCoordinatesForSliderSelection
		&& maxValue + sliderWidthInSliderCoordinates*(1-maxValue/maximum()) > mousePositionInSliderCoordinatesForSliderSelection
		&& currentlySelected == -1
		&& justPushed)
	{
		setSliderPosition(maxValue);
		currentlySelected = 1;
	}

	int minValuePrevious = minValue, maxValuePrevious = maxValue;
	switch (currentlySelected)
	{ 
	case 0:
		if (mousePositionInSliderCoordinates > (maxValue - minDistanceBetweenSliders)){
			(mousePositionInSliderCoordinates + minDistanceBetweenSliders) < maximum() ? maxValue = mousePositionInSliderCoordinates + minDistanceBetweenSliders : maxValue = maximum();
			minValue = maxValue - minDistanceBetweenSliders;
		}
		else minValue = mousePositionInSliderCoordinates;
		if (minValue < minimum()) minValue = minimum();
		setSliderPosition(minValue);
		channelHistogramInformation[*currentIndex].minValue = minValue;
		channelHistogramInformation[*currentIndex].maxValue = maxValue;
		if (minValuePrevious != minValue) emit minimumValueChanged(QString("Minimum: %1").arg(minValue,6));
		if (maxValuePrevious != maxValue) emit maximumValueChanged(QString("Maximum: %1").arg(maxValue,6));
		break;
	case 1:
		if (mousePositionInSliderCoordinates < (minValue + minDistanceBetweenSliders)){
			(mousePositionInSliderCoordinates - minDistanceBetweenSliders)>minimum() ? minValue = mousePositionInSliderCoordinates - minDistanceBetweenSliders : minValue = minimum();
			maxValue = minValue + minDistanceBetweenSliders;
		}
		else maxValue = mousePositionInSliderCoordinates;
		if (maxValue > maximum()) maxValue = maximum();
		setSliderPosition(maxValue);
		channelHistogramInformation[*currentIndex].minValue = minValue;
		channelHistogramInformation[*currentIndex].maxValue = maxValue;
		if (minValuePrevious != minValue) emit minimumValueChanged(QString("Minimum: %1").arg(minValue,6));
		if (maxValuePrevious != maxValue) emit maximumValueChanged(QString("Maximum: %1").arg(maxValue,6));
		break;
	}
	justPushed = false;
//	printf("%d %d %d %d \n", currentlySelected, mousePositionInSliderCoordinates, minValue, maxValue);
}

void MySliderWithMultipleHandles::mouseReleaseEvent(QMouseEvent *ev)
{ Q_UNUSED(ev);
	currentlySelected = -1;
	justPushed = true;
}

void MySliderWithMultipleHandles::mousePressEvent(QMouseEvent *ev)
{ mousePositionInSliderCoordinatesForSliderSelection = minimum() + ((maximum() - minimum()) * ev->x()) / width();
}