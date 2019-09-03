#include "WebAppHelpWidget.h"

#include <Wt/WAnimation.h>
#include <Wt/WPanel.h>
#include <Wt/WText.h>

WebAppHelpWidget::WebAppHelpWidget() : WContainerWidget()
{
	std::string helpString = "You can zoom in on the chart below using ctrl+scroll, or with a pinch movement, and pan it with the scrollwheel, click and drag, or touch and drag. ";
	auto panel = this->addWidget(Wt::cpp14::make_unique<Wt::WPanel>());
	panel->setTitle("Interaction Information:");
	panel->addStyleClass("centered-example");
	panel->setCollapsible(true);

	Wt::WAnimation animation(Wt::AnimationEffect::SlideInFromTop,
		Wt::TimingFunction::EaseOut,
		100);

	panel->setAnimation(animation);
	panel->setCentralWidget(Wt::cpp14::make_unique<Wt::WText>("You can zoom in on the chart below using ctrl+scroll, or with a pinch movement, and pan it with the scrollwheel, click and drag, or touch and drag."));
}
