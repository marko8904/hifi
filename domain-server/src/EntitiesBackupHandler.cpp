//
//  EntitiesBackupHandler.cpp
//  domain-server/src
//
//  Created by Clement Brisset on 2/14/18.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "EntitiesBackupHandler.h"

#include <QDebug>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>

#include <LimitedNodeList.h>
#include <OctreeUtils.h>

EntitiesBackupHandler::EntitiesBackupHandler(QString entitiesFilePath, QString entitiesReplacementFilePath) :
    _entitiesFilePath(entitiesFilePath),
    _entitiesReplacementFilePath(entitiesReplacementFilePath)
{
}

static const QString ENTITIES_BACKUP_FILENAME = "models.json.gz";

void EntitiesBackupHandler::createBackup(QuaZip& zip) {
    QFile entitiesFile { _entitiesFilePath };

    if (entitiesFile.open(QIODevice::ReadOnly)) {
        QuaZipFile zipFile { &zip };
        zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(ENTITIES_BACKUP_FILENAME, _entitiesFilePath));
        zipFile.write(entitiesFile.readAll());
        zipFile.close();
        if (zipFile.getZipError() != UNZ_OK) {
            qCritical().nospace() << "Failed to zip " << ENTITIES_BACKUP_FILENAME << ": " << zipFile.getZipError();
        }
    }
}

void EntitiesBackupHandler::recoverBackup(QuaZip& zip) {
    if (!zip.setCurrentFile(ENTITIES_BACKUP_FILENAME)) {
        qWarning() << "Failed to find" << ENTITIES_BACKUP_FILENAME << "while recovering backup";
        return;
    }
    QuaZipFile zipFile { &zip };
    if (!zipFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open" << ENTITIES_BACKUP_FILENAME << "in backup";
        return;
    }
    auto rawData = zipFile.readAll();

    zipFile.close();

    OctreeUtils::RawOctreeData data;
    if (!OctreeUtils::readOctreeDataInfoFromData(rawData, &data)) {
        qCritical() << "Unable to parse octree data during backup recovery";
        return;
    }

    data.resetIdAndVersion();

    if (zipFile.getZipError() != UNZ_OK) {
        qCritical().nospace() << "Failed to unzip " << ENTITIES_BACKUP_FILENAME << ": " << zipFile.getZipError();
        return;
    }

    QFile entitiesFile { _entitiesReplacementFilePath };

    if (entitiesFile.open(QIODevice::WriteOnly)) {
        entitiesFile.write(data.toGzippedByteArray());
        entitiesFile.close();

        auto nodeList = DependencyManager::get<LimitedNodeList>();
        nodeList->eachMatchingNode([](const SharedNodePointer& otherNode) -> bool {
            return otherNode->getType() == NodeType::EntityServer;
        }, [nodeList](const SharedNodePointer& otherNode) {
            QMetaObject::invokeMethod(nodeList.data(), "killNodeWithUUID", Q_ARG(const QUuid&, otherNode->getUUID()));
        });
    }
}
