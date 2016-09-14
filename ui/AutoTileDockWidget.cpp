#include "AutoTileDockWidget.h"
#include "devices\Microscope.h"
#include "MainWindow.h"
#include "AgentController.h"

namespace fetch{
namespace ui{
  AutoTileDockWidget::AutoTileDockWidget(device::Microscope *dc, MainWindow *parent)
    : QDockWidget("Auto Tile",parent)
  { QWidget *w = new QWidget(this);
    QFormLayout *form = new QFormLayout;
    w->setLayout(form);
    setWidget(w);

	///// Use Current Z Checkbox Control
	QLabel * useCurrentZCheckBoxLable = new QLabel();
	useCurrentZCheckBoxLable->setText("Resume Using Current Z:");
	useCurrentZCheckBoxLable->setVisible(false);

	QCheckBox *useCurrentZCheckBox = new QCheckBox();
	useCurrentZCheckBox->setVisible(false);
	QStateMachine *useCurrentZmachine = new QStateMachine(this);
	QState *doUseCurrentZ = new QState(),
		*doNotUseCurrentZ = new QState();
	doUseCurrentZ->addTransition(useCurrentZCheckBox, SIGNAL(stateChanged(int)), doNotUseCurrentZ);
	doNotUseCurrentZ->addTransition(useCurrentZCheckBox, SIGNAL(stateChanged(int)), doUseCurrentZ);
	doUseCurrentZ->assignProperty(useCurrentZCheckBox, "text", "Will Use Current Z");
	doNotUseCurrentZ->assignProperty(useCurrentZCheckBox, "text", "Will Not Use Current Z");
	useCurrentZmachine->addState(doUseCurrentZ);
	useCurrentZmachine->addState(doNotUseCurrentZ);

	useCurrentZCheckBox->setCheckState(Qt::Unchecked);
	useCurrentZmachine->setInitialState(doNotUseCurrentZ);
	useCurrentZmachine->start();

    parent->_autotile_zoffum_control->createLineEditAndAddToLayout(form);
    parent->_autotile_zmaxmm_control->createLineEditAndAddToLayout(form);
    parent->_autotile_timeoutms_control->createLineEditAndAddToLayout(form);
    parent->_autotile_chan_control->createLineEditAndAddToLayout(form);
    parent->_autotile_intensity_thresh_control->createLineEditAndAddToLayout(form);
    parent->_autotile_area_thresh_control->createLineEditAndAddToLayout(form);
	QObject::connect(parent->_stageController, SIGNAL(useCurrentZSet(bool)), useCurrentZCheckBox, SLOT(setChecked(bool)));
	QObject::connect(useCurrentZCheckBox, SIGNAL(stateChanged(int)), parent->_stageController, SLOT(setUseCurrentZ(int))); //DGA: When the checkbox is clicked, then the toggleUseCurrentZ function of parent->_stageController will be called with true (if checked) or false (if unchecked), which will update stage->useCurrentZ_
	
	//DGA: The following connects the tiling listener signal sig_autoTileImagingStopped to the checkbox slot setEnabled so that whatevever is passed to the signal will be passed to setEnabled
	QObject::connect((parent->_stageController->tiling())->listener(), SIGNAL(sig_autoTileImagingStopped(bool)), useCurrentZCheckBox, SLOT(setVisible(bool)), Qt::QueuedConnection);

    QObject::connect((parent->_stageController->tiling())->listener(), SIGNAL(sig_autoTileImagingStopped(bool)), useCurrentZCheckBoxLable, SLOT(setVisible(bool)), Qt::QueuedConnection);

	form->addRow(useCurrentZCheckBoxLable, useCurrentZCheckBox);
    AgentControllerButtonPanel *btns = new AgentControllerButtonPanel(&parent->_scope_state_controller,&dc->auto_tile_task);
    form->addRow(btns);
  }
}}//fetch::ui