/*
    Copyright 2008 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 27/06/08.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __dialog_file_decl_h__
#define __dialog_file_decl_h__

#include <QDialog>
#include <QAbstractListModel>

#include "ptr.h"
#include "timer.h"
#include "image/header.h"
#include "file/path.h"
#include "file/dicom/tree.h"

class QWidget;
class QTreeView;
class QLineEdit;
class QPushButton;
class QTimer;
class QSortFilterProxyModel;

namespace MR {

  namespace File {
    namespace Dicom {
      class Series;
      class Tree;
    }
  }


  namespace Dialog {

    class FolderModel : public QAbstractListModel 
    {
      Q_OBJECT

      public:
        void add_entries (const QStringList& more);
        void clear ();
        std::string name (int num) const { return (list[num].toAscii().data()); }

        int rowCount (const QModelIndex &parent = QModelIndex()) const;
        QVariant data (const QModelIndex &index, int role) const;
        QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      private:
        QStringList list;
    };


    class FileModel : public QAbstractListModel 
    {
      Q_OBJECT

      public:
        FileModel () : num_dicom_series (0) { }
        void add_entries (const std::vector<std::string>& more);
        void clear ();
        bool is_file (int num) const { return (num >= int (num_dicom_series)); }
        std::string name (int num) const { assert (is_file (num)); return (list[num-num_dicom_series]); }
        RefPtr<MR::File::Dicom::Series> get_dicom_series (size_t index) const;
        bool check_image (const std::string& path);
        void check_dicom (const std::string& path);

        int rowCount (const QModelIndex &parent = QModelIndex()) const;
        QVariant data (const QModelIndex &index, int role) const;
        QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      private:
        std::vector<std::string> list;
        MR::File::Dicom::Tree dicom_tree;
        size_t num_dicom_series;
    };




    class File : public QDialog
    {
      Q_OBJECT

      public:
        File (QWidget* parent, const std::string& message, bool multiselection, bool images_only);
        ~File ();

        void get_selection (std::vector<std::string>& filenames);
        void get_images (VecPtr<Image::Header>& images);

      protected slots:
        void idle_slot ();
        void up_slot ();
        void home_slot ();
        void folder_selected_slot (const QModelIndex& index);
        void file_selected_slot (const QModelIndex& index);
        void update ();

      protected:

        FolderModel* folders;
        FileModel*   files;
        QSortFilterProxyModel* sorted_folders;
        QSortFilterProxyModel* sorted_files;
        QTreeView*   folder_view;
        QTreeView*   files_view;
        QLineEdit*   path_entry;
        QLineEdit*   selection_entry;
        QPushButton* ok_button;
        QTimer*      idle_timer;
        Timer        elapsed_timer;

        bool filter_images, updating_selection;
        size_t current_index;
        Ptr<Path::Dir> dir;

        std::string get_next_file () {
          while (true) {
            std::string entry = dir->read_name();
            if (entry.empty()) return (entry);
            if (entry[0] == '.') continue;
            if (Path::is_dir (Path::join (cwd, entry))) continue;
            return (entry);
          } 
        }

        static std::string cwd;
        static QPoint window_position;
        static QSize window_size;
    };

  }
}

#endif

