#pragma once
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WTableView.h>
#include <Wt/WModelIndex.h>

using namespace Wt;
using namespace Wt::Chart;

namespace {

	/*
	 * A standard item which converts text edits to numbers
	 */
	class NumericItem : public Wt::WStandardItem {
	public:
		virtual std::unique_ptr<Wt::WStandardItem> clone() const override {
			return cpp14::make_unique<NumericItem>();
		}

		virtual void setData(const cpp17::any& data, ItemDataRole role = ItemDataRole::User) override {
			cpp17::any dt;

			if (role == ItemDataRole::Edit) {
				std::string s = asString(data).toUTF8();
				char* endptr;
				double d = strtod(s.c_str(), &endptr);
				if (*endptr == 0) {
					dt = cpp17::any(d);
				}
				else {
					unsigned long long ullV = strtoull(s.c_str(), &endptr, 10);
					if (*endptr == 0) {
						dt = cpp17::any(ullV);
					}
					else {
						dt = data;
					}
				}
			}

			WStandardItem::setData(data, role);
		}
	};

}